import configparser as cp

def get_image_raw_path():
    config = cp.RawConfigParser()   
    config.readfp(open(r'config.cfg'))
    return config.get('DIRECTORIES', 'images_raw')

def get_image_metadata_path():
    config = cp.RawConfigParser()   
    config.readfp(open(r'config.cfg'))
    return config.get('DIRECTORIES', 'images_metadata')

def get_database_backups_path():
    config = cp.RawConfigParser()   
    config.readfp(open(r'config.cfg'))
    return config.get('DIRECTORIES', 'database_backups')

def get_database_path():
    config = cp.RawConfigParser()   
    config.readfp(open(r'config.cfg'))
    return config.get('DIRECTORIES', 'database')

def get_puzzle_data():
    config = cp.RawConfigParser()   
    config.readfp(open(r'config.cfg'))
    return config.get('FILES', 'puzzle_data')
