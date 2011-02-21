.. Installation
.. ============

Requirements
------------

``imposm.parser`` runs with Python 2.5, 2.6 and 2.7. You need the install the `multiprocessing <http://pypi.python.org/pypi/multiprocessing>`_ package if you run Python 2.5.

The PBF parser is written as a C extension and you need to have a C/C++ compiler, the Python libraries and Google Protobuf.

On Ubuntu::

  sudo aptitude install build-essential python-devel protobuf-compiler

Installation
------------

You can install ``imposm.parser`` with ``pip`` or ``easy_install``.

::

  pip install imposm.parser
  
::

  easy_install imposm.parser




