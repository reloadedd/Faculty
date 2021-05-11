# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
import random
import string
from bitarray import bitarray
from bitarray.util import int2ba
from rich import print
from .utils import *


class Key:
    def __init__(self, key: str = None, as_hex: bool = False):
        self.key_space = string.ascii_letters + string.digits
        self.as_hex = as_hex

        if not key:
            self.str_key = self._generate_key()
            self.hex_key = convert_to_hex_from_str(self.str_key)
        else:
            if as_hex:
                self.hex_key = key
                self.str_key = convert_to_str_from_hex(key)
            else:
                if len(key) != 8:
                    self.str_key = self._generate_key()
                    print("[red][ INFO ]:[/red] The length of the key is not "
                          "8! Defaulting to a randomly generated key.")
                else:
                    self.str_key = key
                self.hex_key = convert_to_hex_from_str(self.str_key)

    def _generate_key(self):
        return ''.join(random.choice(self.key_space) for _ in range(8))

    def to_hex(self):
        return self.hex_key

    def __str__(self):
        return self.str_key

    def __getitem__(self, index):
        return str(self)[index]
