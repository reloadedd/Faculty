#!/usr/bin/env python3
# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

from PRNGs import *
from rich import print
from zipfile import ZipFile, ZIP_DEFLATED

FULL_ONES_FILE = 'full_ones.bin'
ZIP_FULL_ONES_FILE = 'full_ones.zip'
BBS_PRNG_OUTPUT_FILE = 'bbs_output_bytes.bin'
ZIP_BBS_PRNG_OUTPUT_FILE = 'bbs_output_bytes.zip'
JACOBI_PRNG_OUTPUT_FILE = 'jacobi_output_bytes.bin'
ZIP_JACOBI_PRNG_OUTPUT_FILE = 'jacobi_output_bytes.zip'


def write_full_ones(file: str) -> None:
    """Write into a file just ones, stored as strings.

    :param file where to save those ones.
    """
    with open(file, 'wb') as write_me:
        for i in range(config.HOW_MANY_NUMBERS_TO_GENERATE):
            write_me.write(int(1).to_bytes(1, byteorder='little'))


def write_prng_output(file: str, prng_output) -> None:
    """Write the output of the PRNG to a file.

    :param file         where to write the output.
    :param prng_output  the output of the PRNG.
    """
    with open(file, 'wb') as write_me:
        for i in prng_output:
            write_me.write(i.to_bytes(1, byteorder='little'))


def archive(file: str, zip_archive: str):
    """Archive a file using ZIP Deflate compression method.

    :param file         what file to archive.
    :param zip_archive  what will be the name of the archive.
    """
    with ZipFile(zip_archive, 'w', compression=ZIP_DEFLATED) as zip_file:
        zip_file.write(file)

    return zip_archive


def compute_compression_rate(file: str, zip_archive) -> float:
    """Compute the compression rate of two files.

    More info: https://en.m.wikipedia.org/wiki/Data_compression_ratio

    :param file:        the uncompressed file.
    :param zip_archive  the same file but compressed.
    :returns            the compression rate.
    """
    uncompressed = Helper.Helper.get_file_size(file)
    compressed = Helper.Helper.get_file_size(archive(file, zip_archive))

    return uncompressed / compressed


def compare_compression_rate(generator: str = 'bbs'):
    """Compare the compression rates of the two PRNGs.

    :param generator    the name of the generator
    """
    full_ones = compute_compression_rate(FULL_ONES_FILE,
                                         ZIP_FULL_ONES_FILE)
    print(f'[magenta][ INFO ]:[/magenta] Compression rate of full ones is '
          f'{full_ones}')

    if generator == 'bbs':
        output = compute_compression_rate(BBS_PRNG_OUTPUT_FILE,
                                          ZIP_BBS_PRNG_OUTPUT_FILE)

        print(f'[magenta][ INFO ]:[/magenta] Compression rate of generated '
              f'sequence (BBS) is {output}')
    elif generator == 'jacobi':
        output = compute_compression_rate(JACOBI_PRNG_OUTPUT_FILE,
                                          ZIP_JACOBI_PRNG_OUTPUT_FILE)

        print(f'[magenta][ INFO ]:[/magenta] Compression rate of generated '
              f'sequence (Jacobi) is {output}')
    else:
        print('[red][ ERROR ]:[/red] The generator must be \'bbs\' or '
              '\'jacobi\'')


def main():
    # Make a file with full ones
    write_full_ones(FULL_ONES_FILE)

    bbs = BlumBlumShub.BlumBlumShub()
    write_prng_output(BBS_PRNG_OUTPUT_FILE, bbs.generate(1000))
    compare_compression_rate('bbs')

    jacobi = Jacobi.Jacobi()
    write_prng_output(JACOBI_PRNG_OUTPUT_FILE, jacobi.generate(1000))
    compare_compression_rate('jacobi')


if __name__ == '__main__':
    main()
