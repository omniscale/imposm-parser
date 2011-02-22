# -:- encoding: utf8 -:-
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

import os
from imposm.parser import OSMParser
from nose.tools import eq_

class ParserTestBase(object):
    osm_filename = None
    ways_filter = None
    nodes_filter = None
    relations_filter = None
    def __init__(self):
        self.nodes = []
        self.coords = []
        self.ways = []
        self.relations = []
    
    def parse_nodes(self, nodes):
        self.nodes.extend(nodes)
    def parse_coords(self, coords):
        self.coords.extend(coords)
    def parse_ways(self, ways):
        self.ways.extend(ways)
    def parse_relations(self, relations):
        self.relations.extend(relations)

    def parse(self):
        parser = OSMParser(2,
            nodes_callback=self.parse_nodes,
            coords_callback=self.parse_coords,
            ways_callback=self.parse_ways,
            relations_callback=self.parse_relations,
            nodes_tag_filter=self.nodes_filter,
            ways_tag_filter=self.ways_filter,
            relations_tag_filter=self.relations_filter,
        )
        osm_filename = os.path.join(os.path.dirname(__file__), self.osm_filename)
        parser.parse(osm_filename)
    
    def test_parse_result(self):
        self.parse()
        eq_(len(self.nodes), 1)
        eq_(self.nodes[0],
            (2, {'name': 'test', 'created_by': 'hand'}, (10.0, 51.0)))
        
        eq_(len(self.coords), 2)
        eq_(self.coords[0], (1, 10.0, 50.0))
        eq_(self.coords[1], (2, 10.0, 51.0))
        
        eq_(len(self.ways), 1)
        eq_(self.ways[0],
            (3, {'highway': 'primary'}, [1, 2]))

        eq_(len(self.relations), 1)
        eq_(self.relations[0],
            (4, {'name': u'Üµlåû†é'}, [(123, 'way', 'outer'), (124, 'way', 'inner')]))

class ParserTestBaseWithFilter(ParserTestBase):
    def nodes_filter(self, tags):
        for tag in tags.keys():
            if tag != 'name':
                del tags[tag]
    
    ways_filter = nodes_filter
    def relations_filter(self, tags):
        tags.clear()
    
    def test_parse_result(self):
        self.parse()
        eq_(len(self.nodes), 1)
        eq_(self.nodes[0],
            (2, {'name': 'test'}, (10.0, 51.0)))
        
        eq_(len(self.coords), 2)
        eq_(self.coords[0], (1, 10.0, 50.0))
        eq_(self.coords[1], (2, 10.0, 51.0))
        
        eq_(len(self.ways), 1)
        eq_(self.ways[0],
            (3, {}, [1, 2]))

        eq_(len(self.relations), 0)
        
class TestXML(ParserTestBase):
    osm_filename = 'test.osm'

class TestBZIP2(ParserTestBase):
    osm_filename = 'test.osm.bz2'

class TestPBF(ParserTestBase):
    osm_filename = 'test.pbf'

class TestXMLWithFilter(ParserTestBaseWithFilter):
    osm_filename = 'test.osm'

class TestPBFWithFilter(ParserTestBaseWithFilter):
    osm_filename = 'test.pbf'
