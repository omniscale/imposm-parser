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

import struct
import sys
import zlib

from marshal import dumps

from imposm.parser.pbf import OSMPBF

SUPPORTED_FEATURES = set(['OsmSchema-V0.6', 'DenseNodes'])


_MEMBERTYPE = {0 : 'node',
               1 : 'way',
               2 : 'relation'}



class PBFParser(object):
    """
    OSM PBF parser.

    :param xxx_callback: 
        callback functions for coords, nodes, ways and relations.
        Each callback function gets called with a list of multiple elements.
    
    :param xxx_filter:
        functions that can manipulate the tag dictionary.
        Nodes and relations without tags will not passed to the callback.
    
    :param marshal:
        return the data as a marshaled string
    """
    def __init__(self, nodes_callback=None, ways_callback=None,
        relations_callback=None, coords_callback=None, nodes_tag_filter=None,
        ways_tag_filter=None, relations_tag_filter=None, marshal=False):
        self.nodes_callback = nodes_callback
        self.ways_callback = ways_callback
        self.relations_callback = relations_callback
        self.coords_callback = coords_callback
        self.nodes_tag_filter = nodes_tag_filter
        self.ways_tag_filter = ways_tag_filter
        self.relations_tag_filter = relations_tag_filter
        self.marshal = marshal
    
    def parse(self, filename, offset, size):
        """
        Parse primitive block from `filename`.
        
        :param filename: path to PBF file
        :param offset: byte offset of the primitive block to parse
        :param size: size in bytes of the primitive block to parse
        """
        reader = PrimitiveBlockParser(filename, offset, size)
        
        if self.nodes_callback or self.coords_callback:
            self.handle_nodes(reader)
        if self.ways_callback:
            self.handle_ways(reader)
        if self.relations_callback:
            self.handle_relations(reader)
    
    def handle_nodes(self, reader):
        nodes = []
        coords = []
        nodes_callback = self.nodes_callback
        coords_callback = self.coords_callback
        for node in reader.nodes():
            if nodes_callback:
                if self.nodes_tag_filter:
                    self.nodes_tag_filter(node[1])
                if node[1]:
                    if self.marshal:
                        nodes.append((node[0], dumps((node[1], node[2]), 2)))
                    else:
                        nodes.append((node[0], node[1], node[2]))
                if len(nodes) >= 256:
                    nodes_callback(nodes)
                    nodes = []
            if coords_callback:
                coords.append((node[0], node[2][0], node[2][1]))
                if len(coords) >= 512:
                    coords_callback(coords)
                    coords = []
        if nodes_callback:
            nodes_callback(nodes)
        if coords_callback:
            coords_callback(coords)

    def handle_ways(self, reader):
        ways = []
        for way in reader.ways():
            if self.ways_tag_filter:
                self.ways_tag_filter(way[1])
            # always return ways, might be needed for relations
            if self.marshal:
                ways.append((way[0], dumps((way[1], way[2]), 2)))
            else:
                ways.append((way[0], way[1], way[2]))
            if len(ways) >= 256:
                self.ways_callback(ways)
                ways = []
        self.ways_callback(ways)

    def handle_relations(self, reader):
        relations = []
        for relation in reader.relations():
            if self.relations_tag_filter:
                self.relations_tag_filter(relation[1])
                if not relation[1]:
                    continue
            if self.marshal:
                relations.append((relation[0], dumps((relation[1], relation[2]), 2)))
            else:
                relations.append((relation[0], relation[1], relation[2]))
            if len(relations) >= 256:
                self.relations_callback(relations)
                relations = []
        self.relations_callback(relations)

def decoded_stringtable(stringtable):
    result = []
    for s in stringtable:
        result.append(s.decode('utf-8'))
    return result

class PrimitiveBlockParser(object):
    """
    Low level PBF primitive block parser.
    
    Parses a single primitive block and handles OSM PBF internals like
    dense nodes, delta encoding, stringtables, etc.
    
    :param filename: path to PBF file
    :param offset: byte offset of the primitive block to parse
    :param size: size in bytes of the primitive block to parse
    
    """
    def __init__(self, filename, blob_pos, blob_size):
        self.pos = filename, blob_pos, blob_size
        data = read_blob_data(filename, blob_pos, blob_size)
        self.primitive_block = OSMPBF.PrimitiveBlock()
        self.primitive_block.ParseFromString(data)
        self.primitivegroup = self.primitive_block.primitivegroup
        self.stringtable = decoded_stringtable(self.primitive_block.stringtable.s)
    
    def __repr__(self):
        return '<PrimitiveBlockParser %r>' % (self.pos, )
    
    def _get_tags(self, element, pos):
        tags = {}
        key = None
        value = None
        keyflag = False
        if pos >= len(element):
            return {}, pos 
        while True:
            key_val = element[pos]
            pos += 1
            if key_val == 0:
                break
            if not keyflag:
                key = key_val
                keyflag = True
            else:
                value = key_val
                tags[self.stringtable[key]] = self.stringtable[value]
                keyflag = False
        return tags, pos
    
    def nodes(self):
        """
        Return an iterator for all *nodes* in this primitive block.
        
        :rtype: iterator of ``(osm_id, tags, (lon, lat))`` tuples
        """
        for group in self.primitivegroup:
            dense = group.dense
            if dense:
                granularity = self.primitive_block.granularity or 100
                lat_offset = self.primitive_block.lat_offset or 0
                lon_offset = self.primitive_block.lon_offset or 0
                coord_scale = 0.000000001
                get_tags = self._get_tags
                ids = dense.id
                lats = dense.lat
                lons = dense.lon
                keys_vals = dense.keys_vals
                last_id = last_lat = last_lon = last_keysvals_pos = 0
                for i in xrange(len(ids)):
                    last_id += ids[i]
                    last_lat += coord_scale * (lat_offset + (granularity * lats[i]))
                    last_lon += coord_scale * (lon_offset + (granularity * lons[i]))
                    tags, last_keysvals_pos = get_tags(keys_vals, last_keysvals_pos)
                    yield (last_id, tags, (last_lon, last_lat))
            nodes = group.nodes
            if nodes:
                for node in nodes:
                    keys, vals = node.keys, node.vals
                    tags = []
                    for i in xrange(len(keys)):
                        tags.append((self.stringtable[keys[i]], self.stringtable[vals[i]]))
                    yield (node.id, tags, (node.lon, node.lat))
    
    def ways(self):
        """
        Return an iterator for all *ways* in this primitive block.
        
        :rtype: iterator of ``(osm_id, tags, [ref1, ref2, ...])`` tuples
        """
        for group in self.primitivegroup:
            ways = group.ways
            if ways:
                for way in ways:
                    keys = way.keys
                    vals = way.vals
                    delta_refs = way.refs
                    
                    tags = {}
                    for i in xrange(len(keys)):
                        tags[self.stringtable[keys[i]]] = self.stringtable[vals[i]]
                    refs = []
                    ref = 0
                    for delta in delta_refs:
                        ref += delta
                        refs.append(ref)
                    yield (way.id, tags, refs)
    
    def relations(self):
        """
        Return an iterator for all *relations* in this primitive block.
        
        :rtype: iterator of ``(osm_id, tags, [(ref1, type, role), ...])`` tuples
        
        """
        for group in self.primitivegroup:
            relations = group.relations
            if relations:
                for relation in relations:
                    members = []
                    memids = relation.memids
                    rel_types = relation.types
                    roles_sids = relation.roles_sid
                    keys = relation.keys
                    vals = relation.vals
                    memid = 0
                    for i in xrange(len(rel_types)):
                        memid += memids[i]
                        members.append((memid, _MEMBERTYPE[rel_types[i]], self.stringtable[roles_sids[i]]))
                    tags = {}
                    for i in xrange(len(keys)):
                        tags[self.stringtable[keys[i]]] = self.stringtable[vals[i]]
                    yield (relation.id, tags, members)
                    
class PBFHeader(object):
    def __init__(self, filename, blob_pos, blob_size):
        data = read_blob_data(filename, blob_pos, blob_size)
        self.header_block = OSMPBF.HeaderBlock()
        self.header_block.ParseFromString(data)
        
    def required_features(self):
        return set(self.header_block.required_features)


def read_blob_data(filename, blob_pos, blob_size):
    """
    Returns the (unzipped) blob data from filename and position.
    """
    with open(filename, 'rb') as f:
        f.seek(blob_pos)
        blob_data = f.read(blob_size)
        
    blob = OSMPBF.Blob()
    blob.ParseFromString(blob_data)
    raw_data = blob.raw
    if raw_data:
        return raw_data
    return zlib.decompress(blob.zlib_data)

import time

class PBFFile(object):
    """
    OSM PBF file reader.
    
    Parses the low-level file structure with header sizes,
    offsets and blob headers.
    
    :param filename: path to the PBF file
    """
    def __init__(self, filename):
        self.filename = filename
        self.file = open(filename, 'rb')
        self.next_blob_pos = self.prev_blob_pos = 0
        header_offsets = self._skip_header()
        self.header = PBFHeader(self.filename, header_offsets['blob_pos'], header_offsets['blob_size'])
        self.check_features()
    
    def check_features(self):
        missing_features = self.header.required_features().difference(SUPPORTED_FEATURES)
        if missing_features:
            raise NotImplementedError(
                '%s requires features not implemented by this parser: %s' %
                (self.filename, ', '.join(missing_features))
            )
    
    def _skip_header(self):
        return self.blob_offsets().next()
    
    def seek(self, pos):
        self.next_blob_pos = pos
    
    def rewind(self):
        self.next_blob_pos = self.prev_blob_pos
    
    def blob_offsets(self):
        """
        Returns an iterator of the blob offsets in this file.
        
        Each offsets is stored in a dictionary with:
        
        - `filename` the path of this PBF file.
        - `blob_pos` the byte offset
        - `blob_size` the size of this blob in bytes
        """
        while True:
            self.file.seek(self.next_blob_pos)
            
            blob_header_size = self._blob_header_size()
            if not blob_header_size: break
            
            blob_size = self._blob_size(self.file.read(blob_header_size))
            blob_pos = self.next_blob_pos + 4 + blob_header_size
            blob_header_pos=self.next_blob_pos,
            prev_blob_header_pos = self.prev_blob_pos
            self.prev_blob_pos = self.next_blob_pos
            self.next_blob_pos = blob_pos + blob_size
            yield dict(blob_pos=blob_pos, blob_size=blob_size,
                blob_header_pos=blob_header_pos,
                prev_blob_header_pos=prev_blob_header_pos,
                filename=self.filename)
    
    def primitive_block_parsers(self):
        """
        Returns an iterator of PrimitiveBlockParser.
        """
        for pos in self.blob_offsets():
            yield PrimitiveBlockParser(self.filename, pos['blob_pos'], pos['blob_size'])
    
    def _blob_size(self, data):
        blob_header = OSMPBF.BlobHeader()
        blob_header.ParseFromString(data)
        return blob_header.datasize
        
    def _blob_header_size(self):
        bytes = self.file.read(4)
        if bytes: 
            return struct.unpack('!i', bytes)[0]
        return None


def read_pbf(filename):
    pbf = PBFFile(filename)
    for block in pbf.primitive_block_parsers():
        for node in block.nodes():
            pass
        for way in block.ways():
            pass
        for relation in block.relations():
            pass

if __name__ == '__main__':
    from timeit import Timer
    n = 1
    r = 1
    print "reading %s, number of calls: %d, repeat: %d" %(sys.argv[1],n,r)
    t = Timer("read_pbf(sys.argv[1])", "from __main__ import read_pbf")
    times = t.repeat(r,n)
    avrg_times = []
    for time in times:
        avrg_times.append(time/n)                  
    print "avrg time/call: %f" %(min(avrg_times))
