imposm.parser - OpenStreetMap XML/PBF parser for Python
=======================================================

``imposm.parser`` is a Python library that parses OpenStreetMap data in `XML <http://wiki.openstreetmap.org/wiki/.osm>`_ and `PBF <http://wiki.openstreetmap.org/wiki/PBF_Format>`_ format.

It has a simple API and it is fast and easy to use. It also works across multiple CPU/cores for extra speed.

It is developed and supported by `Omniscale <http://omniscale.com>`_ and released under the `Apache Software License 2.0 <http://www.apache.org/licenses/LICENSE-2.0>`_.

Example
-------

Here is an example that parses an OSM file and counts all ways that are tagged as a highway.
::

  from imposm.parser import OSMParser

  # simple class that handles the parsed OSM data.
  class HighwayCounter(object):
      highways = 0

      def ways(self, ways):
          # callback method for ways
          for osmid, tags, refs in ways:
              if 'highway' in tags:
                self.highways += 1

  # instantiate counter and parser and start parsing
  counter = HighwayCounter()
  p = OSMParser(concurrency=4, ways_callback=counter.ways)
  p.parse('germany.osm.pbf')

  # done
  print counter.highways


Source and issue tracker
------------------------

Source code and issue tracker are available at `<https://github.com/omniscale/imposm-parser>`_.

