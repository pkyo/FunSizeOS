"""
The 1.44MB 3.5-inch floppy disk was a common format from the 1980s to the early 2000s.

Here are some key details about the format and structure of a 1.44MB floppy disk:

Storage Capacity: A floppy disk holds 1.44MB of data.
    In reality, due to the way storage units and counts work,
    the actual usable storage space is about 1.41MB or 1,474,560 bytes.

Disk Structure: A standard 1.44MB floppy disk is double-sided with 80 tracks per side.
    Each track is divided into 18 sectors.
    Each sector can hold 512 bytes of data.
"""

import os

if __name__ == '__main__':
    current_file = os.path.abspath(__file__)

    current_directory = os.path.dirname(current_file)

    project_root = os.path.dirname(current_directory)

    source_file = f'{project_root}/build/hello/hello.o'
    target_file = f'{project_root}/hello.img'
    floppy_size = 1474560  # 2 * 80 * 18 * 512 = 1.44MB

    # Create a bytearray to hold the floppy disk image data
    floppy_image = bytearray([0] * floppy_size)

    with open(source_file, "rb") as file:
        boot_data = file.read()

    floppy_image[:len(boot_data)] = boot_data

    with open(target_file, "wb") as file:
        file.write(floppy_image)
