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

import contextlib
import multiprocessing
import subprocess

try:
    from setproctitle import setproctitle
    setproctitle
except ImportError:
    setproctitle = lambda x: None

def default_concurrency():
    return multiprocessing.cpu_count()

def bzip_reader(filename):
    p = subprocess.Popen(['bunzip2', '-c', filename], bufsize=-1, stdout=subprocess.PIPE)
    return p.stdout

@contextlib.contextmanager
def fileinput(filename):
    if filename.endswith('bz2'):
        yield bzip_reader(filename)
    else:
        fh = open(filename, 'rb')
        yield fh
        fh.close()

def estimate_records(files):
    records = 0
    for f in files:
        fsize = os.path.getsize(f)
        if f.endswith('.bz2'):
            fsize *= 11 # observed bzip2 compression factor on osm data
        if f.endswith('.pbf'):
            fsize *= 15 # observed pbf compression factor on osm data
        records += fsize/200
    
    return int(records)