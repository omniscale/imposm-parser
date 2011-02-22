# Copyright 2011 Omniscale GmbH & Co. KG
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import with_statement

import multiprocessing
import sys
import time

from Queue import Empty

from imposm.parser.util import default_concurrency, fileinput, setproctitle

class OSMParser(object):
    """
    High-level OSM parser.
    
    :param concurrency:
        number of parser processes to start. Defaults to the number of CPUs.
    :param xxx_callback: 
        callback functions for coords, nodes, ways and relations.
        Each callback function gets called with a list of multiple elements.
        See :ref:`callback concepts <concepts_callbacks>`.
    
    :param xxx_filter:
        functions that can manipulate the tag dictionary.
        Nodes and relations without tags will not passed to the callback.
        See :ref:`tag filter concepts <concepts_tag_filters>`.
    
    """
    def __init__(self, concurrency=None, nodes_callback=None, ways_callback=None,
        relations_callback=None, coords_callback=None, nodes_tag_filter=None,
        ways_tag_filter=None, relations_tag_filter=None, marshal_elem_data=False):
        self.concurrency = concurrency or default_concurrency()
        assert self.concurrency >= 1
        self.nodes_callback = nodes_callback
        self.ways_callback = ways_callback
        self.relations_callback = relations_callback
        self.coords_callback = coords_callback
        self.nodes_tag_filter = nodes_tag_filter
        self.ways_tag_filter = ways_tag_filter
        self.relations_tag_filter = relations_tag_filter
        self.marshal_elem_data = marshal_elem_data
    
    def parse(self, filename):
        """
        Parse the given file. Detects the filetype based on the file suffix.
        Supports ``.pbf``, ``.osm`` and ``.osm.bz2``.
        """
        if filename.endswith('.pbf'):
            return self.parse_pbf_file(filename)
        elif filename.endswith(('.osm', '.osm.bz2')):
            return self.parse_xml_file(filename)
        else:
            raise NotImplementedError('unknown file extension')
        
    def parse_pbf_file(self, filename):
        """
        Parse a PBF file.
        """
        from imposm.parser.pbf.multiproc import PBFMultiProcParser
        return self._parse(filename, PBFMultiProcParser)
    
    def parse_xml_file(self, filename):
        """
        Parse a XML file.
        Supports BZip2 compressed files if the filename ends with ``.bz2``.
        """
        from imposm.parser.xml.multiproc import XMLMultiProcParser
        with fileinput(filename) as input:
            return self._parse(input, XMLMultiProcParser)
    
    def _parse(self, input, parser_class):
        queues_callbacks = {}
        if self.coords_callback:
            queues_callbacks['coords'] = (multiprocessing.JoinableQueue(512),
                                          self.coords_callback)
        if self.nodes_callback:
            queues_callbacks['nodes'] = (multiprocessing.JoinableQueue(128),
                                         self.nodes_callback)
        if self.ways_callback:
            queues_callbacks['ways'] = (multiprocessing.JoinableQueue(128),
                                        self.ways_callback)
        if self.relations_callback:
            queues_callbacks['relations'] = (multiprocessing.JoinableQueue(128),
                                             self.relations_callback)

        def parse_it():
            setproctitle('imposm parser')
            queues = dict([(type, q) for type, (q, c) in queues_callbacks.items()])
            
            parser = parser_class(self.concurrency,
                ways_queue=queues.get('ways'),
                coords_queue=queues.get('coords'),
                nodes_queue=queues.get('nodes'),
                relations_queue=queues.get('relations'),
                marshal_elem_data=self.marshal_elem_data
            )
            parser.nodes_tag_filter = self.nodes_tag_filter
            parser.ways_tag_filter = self.ways_tag_filter
            parser.relations_tag_filter = self.relations_tag_filter
            parser.parse(input)
            for q in queues.values():
                q.put(None)
            
        proc = multiprocessing.Process(target=parse_it)
        proc.start()
        
        while queues_callbacks:
            processed = False
            for items_type, (queue, callback) in queues_callbacks.items():
                try:
                    items = None
                    while True:
                        items = queue.get_nowait()
                        if items is None:
                            queue.task_done()
                            del queues_callbacks[items_type]
                            break
                        else:
                            callback(items)
                    if items:
                        processed = True
                except Empty:
                    pass
            if not processed:
                # wait a ms if all queues were empty
                # to give the parser a chance to fill them up
                time.sleep(0.001)
        proc.join()
