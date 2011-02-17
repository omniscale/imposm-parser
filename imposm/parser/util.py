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