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