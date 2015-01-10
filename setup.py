import errno
import platform
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.errors import DistutilsPlatformError

import subprocess

class build_ext_with_protpbuf(build_ext):
    def run(self):
        try:
            proc = subprocess.Popen(
                ['protoc', '--cpp_out', 'imposm/parser/pbf/', 'osm.proto'],
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        except OSError, ex:
            if ex.errno == errno.ENOENT:
                print ("Could not find protoc command. Make sure protobuf is "
                    "installed and your PATH environment is set.")
                raise DistutilsPlatformError("Failed to generate protbuf "
                    "CPP files with protoc.")
            else:
                raise
        out = proc.communicate()[0]
        result = proc.wait()
        if result != 0:
            print "Error during protbuf files generation with protoc:"
            print out
            raise DistutilsPlatformError("Failed to generate protbuf "
                "CPP files with protoc.")
        build_ext.run(self)


install_requires = []
if tuple(map(str, platform.python_version_tuple())) < ('2', '6'):
    install_requires.append('multiprocessing>=2.6')

setup(
    name='imposm.parser',
    version="1.0.8a",
    description='Fast and easy OpenStreetMap XML/PBF parser.',
    long_description=open('README.rst').read() + open('CHANGES').read(),
    author='Oliver Tonnhofer',
    author_email='olt@omniscale.de',
    url='http://imposm.org/docs/imposm.parser/latest/',
    license='Apache Software License 2.0',
    packages=find_packages(),
    namespace_packages = ['imposm'],
    include_package_data=True,
    package_data = {'': ['*.xml', '*.osm', '*.osm.bz2']},
    install_requires=install_requires,
    classifiers=[
        "Development Status :: 4 - Beta",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: OS Independent",
        "Programming Language :: C",
        "Programming Language :: C++",
        "Programming Language :: Python :: 2.5",
        "Programming Language :: Python :: 2.6",
        "Programming Language :: Python :: 2.7",
        "Topic :: Software Development :: Libraries",
        "Topic :: Scientific/Engineering :: GIS",
    ],
    ext_modules=[
        Extension("imposm.parser.pbf.OSMPBF",
            ["imposm/parser/pbf/osm.cc", "imposm/parser/pbf/osm.pb.cc"], libraries=['protobuf']),
    ],
    cmdclass={'build_ext':build_ext_with_protpbuf},
)
