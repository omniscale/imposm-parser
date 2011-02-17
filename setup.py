from setuptools import setup, Extension, find_packages

setup(
    name='imposm.parser',
    version="0.1.0",
    description='Fast OSM XML/PBF reader.',
    #long_description=open('README.txt').read(),
    author='Oliver Tonnhofer',
    author_email='olt@omniscale.de',
    #url='http://mapproxy.org',
    #license='GNU Affero General Public License v3 (AGPLv3)',
    packages=find_packages(),
    namespace_packages = ['imposm'],
    ext_modules=[
        Extension("imposm.parser.pbf.OSMPBF",
            ["imposm/parser/pbf/osm.cc", "imposm/parser/pbf/osm.pb.cc"], libraries=['protobuf']),
    ],
)