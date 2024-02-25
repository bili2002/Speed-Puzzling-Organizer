import python_files.config_reader as config_reader
from PIL import Image
import os


def create_metadata(image_file_name: str):
    def get_image_file(image_file_name: str) -> Image:
        raw_path = config_reader.get_image_raw_path()
        return Image.open(f"{raw_path}/{image_file_name}", 'r')


    def get_meta_file(image_file_name: str):
        metadata_path = config_reader.get_image_metadata_path()
        image_name = os.path.splitext(image_file_name)[0]
        return open(f"{metadata_path}/{image_name}.ppm", "w")


    def write_size(meta_file, image_file: Image):
        h, w = image_file.size
        meta_file.write(f"P3 {h} {w} 255\n")
        

    def write_pixels(meta_file, image_file: Image):
        print_set = lambda el: ' '.join(map(str, el))

        pixels = list(image_file.getdata())
        pixels_string = '\n'.join(map(print_set, pixels))
        
        meta_file.write(pixels_string)


    meta_file = get_meta_file(image_file_name)
    image_file = get_image_file(image_file_name)

    write_size(meta_file, image_file)
    write_pixels(meta_file, image_file)
    meta_file.write('\n')
