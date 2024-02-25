import configparser as cp

def get_image_raw_path():
    config = cp.RawConfigParser()   
    config.readfp(open(r'config.cfg'))
    return config.get('DIRECTORIES', 'images_raw')

def get_image_metadata_path():
    config = cp.RawConfigParser()   
    config.readfp(open(r'config.cfg'))
    return config.get('DIRECTORIES', 'images_metadata')
