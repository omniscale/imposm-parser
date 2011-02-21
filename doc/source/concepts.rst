Concepts
========

To use ``imposm.parser`` you need to understand three basic concepts: Types, Callbacks and Filter

Types
-----

.. note:: In this document Node, Way, Relation with a capital refer to the OSM types and `node`, `way`, `relation` refer to the Imposm types.

OSM has three fundamental element types: Nodes, Ways and Relations. ``imposm.parser`` distinguishes the OSM Nodes between `coords` and `nodes`.

`coords` only store coordinates and there are `coords` for *every* OSM Node. `nodes` also store tags and there are *only* `nodes` for OSM Nodes *with* tags. 


coords
~~~~~~

A tuple with the OSM ID, the longitude and latitude of that node.

::
  
  (4234432, 175.2, -32.1)

``imposm.parser`` will return a `coord` for each OSM Node, even if this OSM Node is also a `node` (i.e. it has tags).

nodes
~~~~~

A tuple with the OSM ID, a tags dictionary and a nested tuple with the longitude and latitude of that node.

::

  (982347, {'name': 'Somewhere', 'place': 'village'}, (-120.2, 23.21))


ways
~~~~

A tuple with the OSM ID, a tags dictionary and a list of references.

::

  (87644, {'name': 'my way', 'highway': 'path'}, [123, 345, 567])

relations
~~~~~~~~~

A tuple with the OSM ID, a tags dictionary and a list of member tuples.
Each member tuple contains the reference, the type (one of `'node'`, `'way'`, `'relation'`) and the role.

::

  (87644, {'type': 'multipolygon'}, [(123, 'way', 'outer'), (234, 'way', 'inner')])

.. _concepts_callbacks:

Callbacks
---------

The parser takes four callback functions for each data type (`coords`, `nodes`, `ways` and `relations`). The callbacks are optional, i.e. you don't need to pass a relations callback if you are not interested in relations.

The functions should expect a list with zero or more items of the corresponding type.

Here is an example callback that prints the coordinates of all Nodes.

::

  def coords_callback(coords):
    for osm_id, lon, lat in coords:
      print '%s %.4f %.4f' % (osm_id, lon, lat)


.. _concepts_tag_filters:

Tag filters
-----------

Tag filter are functions that manipulate tag dictionaries. The functions should modify the dictionary in-place, the return value is ignored.

Elements will be handled different, if you remove all tags from the dictionary. `nodes` and `relations` with empty tags will not be returned, but `ways` will be, since they might be needed for building relations.

Here is an example filter that filters the tags with a whitelist.
::

  whitelist = set(('name', 'place', 'amenity'))
  
  def tag_filter(tags):
    for key in tags.keys():
      if key not in whitelist:
        del tags[key]
    if 'name' in tags and len(tags) == 1:
      # tags with only a name have no information
      # how to handle this element
      del tags['name']
  

