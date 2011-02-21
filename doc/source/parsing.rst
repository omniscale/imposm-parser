Parsing API
===========

Imposm comes with a single ``OSMParser`` class that implements a simple to use, callback-based parser for OSM files.

It supports `XML <http://wiki.openstreetmap.org/wiki/.osm>`_ and `PBF <http://wiki.openstreetmap.org/wiki/PBF_Format>`_ files. It also supports BZip2 compressed XML files.

Concurrency
~~~~~~~~~~~

The parser uses multiprocessing to distribute the parsing across multiple CPUs. This does work with PBF as well as XML files.

You can pass the ``concurrency`` as an argument to ``OSMParser`` and it defaults to the number of CPU and cores of the host system. ``concurrency`` defines the number of parser processes. The main process where the callbacks are handled and the decompression (if you have a .bzip2 file) are handled in additional processes. So you might get better results if you reduce this number on systems with more than two cores.

You can double the number on systems with hyper threading CPUs.


API
~~~

.. module:: imposm.parser

.. autoclass:: OSMParser
  :members:
