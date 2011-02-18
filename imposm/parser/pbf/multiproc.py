from .parser import PBFFile, PBFParser

import multiprocessing

try:
    from setproctitle import setproctitle
    setproctitle
except ImportError:
    setproctitle = lambda x: None

class PBFParserProcess(PBFParser, multiprocessing.Process):
    def __init__(self, pos_queue, *args, **kw):
        self.pos_queue = pos_queue
        PBFParser.__init__(self, *args, **kw)
        multiprocessing.Process.__init__(self)
    
    def run(self):
        setproctitle('OSM PBF Parser')
        while True:
            pos = self.pos_queue.get()
            if pos is None:
                self.pos_queue.task_done()
                break
            
            self.parse(pos['filename'], offset=pos['blob_pos'],
                    size=pos['blob_size'])
            self.pos_queue.task_done()

class PBFMultiProcParser(object):
    nodes_tag_filter = None
    ways_tag_filter = None
    relations_tag_filter = None
    
    def __init__(self, pool_size, nodes_queue=None, ways_queue=None,
        relations_queue=None, coords_queue=None, marshal_elem_data=False):
        self.pool_size = pool_size
        self.nodes_callback = nodes_queue.put if nodes_queue else None
        self.ways_callback = ways_queue.put if ways_queue else None
        self.relations_callback = relations_queue.put if relations_queue else None
        self.coords_callback = coords_queue.put if coords_queue else None
        self.marshal = marshal_elem_data
    def parse(self, filename):
        pos_queue = multiprocessing.JoinableQueue(32)
        pool = []
        for _ in xrange(self.pool_size):
            proc = PBFParserProcess(pos_queue, nodes_callback=self.nodes_callback,
                coords_callback=self.coords_callback, ways_callback=self.ways_callback,
                relations_callback=self.relations_callback,
                nodes_tag_filter=self.nodes_tag_filter,
                ways_tag_filter=self.ways_tag_filter,
                relations_tag_filter=self.relations_tag_filter,
                marshal=self.marshal
            )
            pool.append(proc)
            proc.start()
        
        reader = PBFFile(filename)
        
        for pos in reader.blob_offsets():
            pos_queue.put(pos)
        
        pos_queue.join()

        for proc in pool:
            pos_queue.put(None)
        for proc in pool:
            proc.join()

if __name__ == '__main__':
    import sys

    def count_proc(type, queue):
        def count():
            count = 0
            while True:
                nodes = queue.get()
                if nodes is None:
                    queue.task_done()
                    break
                count += len(nodes)
                queue.task_done()
            print type, count
        return count
    
    
    nodes_queue = multiprocessing.JoinableQueue(128)
    ways_queue = multiprocessing.JoinableQueue(128)
    relations_queue = multiprocessing.JoinableQueue(128)
    
    procs = [
        multiprocessing.Process(target=count_proc('nodes', nodes_queue)),
        multiprocessing.Process(target=count_proc('ways', ways_queue)),
        multiprocessing.Process(target=count_proc('relations', relations_queue))
    ]
    for proc in procs:
        proc.start()
    
    parser = PBFMultiProcParser(2, nodes_queue=nodes_queue,
        ways_queue=ways_queue, relations_queue=relations_queue)
    parser.parse(sys.argv[1])
    
    nodes_queue.put(None)
    ways_queue.put(None)
    relations_queue.put(None)

    for proc in procs:
        proc.join()