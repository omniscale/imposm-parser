import contextlib
import multiprocessing
import subprocess

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