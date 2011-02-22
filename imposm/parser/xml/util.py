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
from xml.etree import cElementTree as ET
from contextlib import contextmanager

def iterparse(fileobj):
    """
    Return root object and iterparser for given ``fileobj``. 
    """
    context = ET.iterparse(fileobj, events=("start", "end"))
    context = iter(context)
    _event, root = context.next()
    return root, context

@contextmanager
def log_file_on_exception(xml):
    try:
        yield
    except SyntaxError, ex:
        import tempfile
        fd_, filename = tempfile.mkstemp('.osm')
        xml.seek(0)
        with open(filename, 'w') as f:
            f.write(xml.read())
        print 'SyntaxError in xml: %s, (stored dump %s)' % (ex, filename)