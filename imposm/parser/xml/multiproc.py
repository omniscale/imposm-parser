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

import mmap
import multiprocessing
import re

from Queue import Empty

from imposm.parser.xml.parser import XMLParser
from imposm.parser.util import setproctitle

KiB = 1024
MiB = 1024*KiB

READ_SIZE = 512*KiB


class MMapReader(object):
    def __init__(self, m, size):
        self.m = m
        self.m.seek(0)
        self.size = size
    
    def read(self, size=None):
        if size is None:
            size = self.size - self.m.tell()
        else:
            size = min(self.size - self.m.tell(), size)
        return self.m.read(size)
        
    def readline(self):
        cur_pos = self.m.tell()
        if cur_pos >= self.size:
            return
        nl_pos = self.m.find('\n')
        self.m.seek(cur_pos)
        return self.m.read(nl_pos-cur_pos)
    
    def seek(self, n):
        self.m.seek(n)

class XMLParserProcess(XMLParser, multiprocessing.Process):
    def __init__(self, mmap_pool, mmap_queue, *args, **kw):
        self.mmap_pool = mmap_pool
        self.mmap_queue = mmap_queue
        XMLParser.__init__(self, *args, **kw)
        multiprocessing.Process.__init__(self)
        self.daemon = True

    def run(self):
        setproctitle('imposm xml parser')
        while True:
            mmap_idx, size = self.mmap_queue.get()
            if mmap_idx is None:
                self.mmap_queue.task_done()
                break
            xml = MMapReader(self.mmap_pool.get(mmap_idx), size)
            self.parse(xml)
            self.mmap_queue.task_done()
            self.mmap_pool.free(mmap_idx)



class XMLMultiProcParser(object):
    nodes_tag_filter = None
    ways_tag_filter = None
    relations_tag_filter = None
    
    def __init__(self, pool_size, nodes_queue=None, ways_queue=None,
        relations_queue=None, coords_queue=None, marshal_elem_data=False):
        self.pool_size = pool_size
        self.pool = []
        self.nodes_callback = nodes_queue.put if nodes_queue else None
        self.ways_callback = ways_queue.put if ways_queue else None
        self.relations_callback = relations_queue.put if relations_queue else None
        self.coords_callback = coords_queue.put if coords_queue else None
        xml_chunk_size=READ_SIZE
        self.mmap_pool = MMapPool(pool_size*8, xml_chunk_size*8)
        self.mmap_queue = multiprocessing.JoinableQueue(8)
        self.marshal_elem_data = marshal_elem_data
        
    def parse(self, stream):
        assert not self.pool
        
        for _ in xrange(self.pool_size):
            proc = XMLParserProcess(self.mmap_pool, self.mmap_queue, nodes_callback=self.nodes_callback,
                coords_callback=self.coords_callback, ways_callback=self.ways_callback,
                relations_callback=self.relations_callback,
                nodes_tag_filter=self.nodes_tag_filter,
                ways_tag_filter=self.ways_tag_filter,
                relations_tag_filter=self.relations_tag_filter,
                marshal_elem_data=self.marshal_elem_data,
            )
            self.pool.append(proc)
            proc.start()
        
        chunker = XMLChunker(stream, self.mmap_pool, xml_chunk_size=READ_SIZE)
        chunker.read(self.mmap_queue, coords_callback=self.coords_callback)
        
        self.mmap_queue.join()
        for proc in self.pool:
            self.mmap_queue.put((None, None))
        for proc in self.pool:
            proc.join()
        

class MMapPool(object):
    """
    Manages multiple mmap files.
    The mmap files can be read and written in different processes.
    """
    def __init__(self, n, mmap_size):
        self.n = n
        self.mmap_size = mmap_size
        self.pool = [mmap.mmap(-1, mmap_size) for _ in range(n)]
        self.free_mmaps = set(range(n))
        self.free_queue = multiprocessing.JoinableQueue()
        
    def new(self):
        """
        Return a free mmap file.
        
        :returns: index, mmap file
        """
        if not self.free_mmaps:
            self.free_mmaps.add(self.free_queue.get())
            self.free_queue.task_done()
        while True:
            # fetch unless free_queue is empty
            try:
                self.free_mmaps.add(self.free_queue.get_nowait())
                self.free_queue.task_done()
            except Empty:
                break
        mmap_idx = self.free_mmaps.pop()
        return mmap_idx, self.pool[mmap_idx]
    
    def join(self):
        while len(self.free_mmaps) < self.n:
            self.free_mmaps.add(self.free_queue.get())
            self.free_queue.task_done()

    def get(self, idx):
        """
        Return mmap file with `idx`.
        """
        return self.pool[idx]

    def free(self, idx):
        """
        Mark mmap file with `idx` as free.
        """
        self.free_queue.put(idx)

class XMLChunker(object):
    """
    Reads and chunks OSM XML file.
    
    Reads OSM XML from `stream` and writes chunks of it into mmap files from
    the `mmap_pool`.
    
    :params xml_chunk_size: chunk XML after this many bytes
    """
    def __init__(self, stream, mmap_pool, xml_chunk_size):
        self.stream = stream
        self.size = xml_chunk_size
        self._last_line = None
        self.mmap_pool = mmap_pool
        self.current_mmap_idx = 0
        self._skip_header()

    def _skip_header(self):
        for line in self.stream:
            if line.lstrip().startswith('<node '):
                self._last_line = line
                return

    def _new_xml_outstream(self):
        self.current_mmap_idx, stream = self.mmap_pool.new()
        stream.seek(0)
        stream.write("<osm xmlns:xapi='http://www.informationfreeway.org/xapi/0.6'>")
        return stream

    def _finished_xml_outstream(self, last_line, stream):
        if '</osm' not in last_line:
            stream.write('</osm>\n')
        return self.current_mmap_idx, stream.tell()

    def read(self, mmaps_queue, coords_callback=None):
        """
        Read and chunk all 
        """
        coord_node_match = None
        xml_nodes = self._new_xml_outstream()
        coords = []
        coord_node_re_match = re.compile(r'^\s*<node id="(\d+)" .*lat="([-0-9.]+)" '
                                          'lon="([-0-9.]+)".*/>').match
        xml_nodes.write(self._last_line)
        split = False
        line = ''
        for line in self.stream:
            if coords_callback:
                coord_node_match = coord_node_re_match(line)
                if coord_node_match:
                    osm_id, lat, lon = coord_node_match.groups()
                    coords.append((int(osm_id), float(lon), float(lat)))
                    if len(coords) >= 512:
                        coords_callback(coords)
                        coords = []
                else:
                    xml_nodes.write(line)
            else:
                xml_nodes.write(line)
            if split:
                if (line.lstrip().startswith('</')
                    or (coords_callback and coord_node_match)
                    or (not coords_callback and coord_node_re_match(line))):
                    mmaps_queue.put(self._finished_xml_outstream(line, xml_nodes))
                    xml_nodes = self._new_xml_outstream()
                    split = False
            elif xml_nodes.tell() > self.size:
                split = True
        if coords_callback:
            coords_callback(coords)
        
        mmaps_queue.put(self._finished_xml_outstream(line, xml_nodes))

if __name__ == '__main__':
    import sys


    def count_proc(type, queue):
        def count():
            count = 0
            while True:
                nodes = queue.get()
                if nodes is None:
                    queue.task_done()
                    break
                count += len(nodes)
                queue.task_done()
            print type, count
        return count
    
    
    nodes_queue = multiprocessing.JoinableQueue(128)
    ways_queue = multiprocessing.JoinableQueue(128)
    relations_queue = multiprocessing.JoinableQueue(128)

    
    procs = [
        multiprocessing.Process(target=count_proc('nodes', nodes_queue)),
        multiprocessing.Process(target=count_proc('ways', ways_queue)),
        multiprocessing.Process(target=count_proc('relations', relations_queue))
    ]
    for proc in procs:
        proc.start()
    
    parser = XMLMultiProcParser(open(sys.argv[1]), 2, nodes_queue=nodes_queue,
        ways_queue=ways_queue, relations_queue=relations_queue)
    parser.start()
    
    nodes_queue.put(None)
    nodes_queue.join()
    
    ways_queue.put(None)
    ways_queue.join()
    relations_queue.put(None)
    relations_queue.join()

    for proc in procs:
        proc.join()