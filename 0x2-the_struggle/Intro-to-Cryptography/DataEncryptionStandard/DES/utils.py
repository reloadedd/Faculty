# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

from bitarray import bitarray
from bitarray.util import int2ba


def convert_to_str_from_hex(what: str):
    chars = []
    for i in range(0, len(what) - 1, 2):
        chars.append(chr(int(what[i] + what[i + 1], 16)))

    return ''.join(chars)


def convert_to_hex_from_str(what: str):
    return ''.join(list(hex(ord(_))[2:] for _ in what))


def convert_str_to_binary(what: str, as_hex: bool = False):
    bit_list = bitarray()

    if not as_hex:
        for symbol in what:
            bit_symbol = bitarray()
            bit_symbol.frombytes(symbol.encode('ascii'))
            bit_list += bit_symbol
    else:
        for i in range(0, len(what) - 1, 2):
            bit_list += (int2ba(int(what[i] + what[i + 1], 16), 8))
    return bit_list
