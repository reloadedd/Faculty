# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
from .Key import Key
from .constants import *
from .utils import *
from bitarray import bitarray
from bitarray.util import ba2int, int2ba, ba2hex
from itertools import permutations


class DESEncryptor:
    def __init__(self, plaintext: str, key: str = None, as_hex: bool = False):
        self.key = Key(key, as_hex)
        self.str_key = str(self.key)
        if as_hex:
            if not key:
                self.bin_key = convert_str_to_binary(self.key.to_hex(), as_hex)
            else:
                self.bin_key = convert_str_to_binary(key, as_hex)
        else:
            self.bin_key = convert_str_to_binary(self.str_key)
        self.mode = "ECB"
        self.plaintext = plaintext
        self.as_hex = as_hex
        self.ciphertext = ''
        self.left_sides = []    # Represent L_i
        self.right_sides = []   # Represent R_i
        self.c_list = []        # Represent C_i
        self.d_list = []        # Represent D_i
        self.keys = []

    def __str__(self):
        return f"DES Symmetric Encryption Algorithm using {self.mode} mode " \
               f"with 64-bit key length"

    @staticmethod
    def create_binary_block(string):
        bit_list = bitarray()
        for symbol in string:
            bit_symbol = bitarray()
            bit_symbol.frombytes(symbol.encode('ascii'))
            bit_list += bit_symbol

        return bit_list

    def decrypt(self):
        if not self.ciphertext:
            print('[red][ ERROR ]:[/red] Nothing to decrypt!')
            return

        self.left_sides = []
        self.right_sides = []
        self.c_list = []
        self.d_list = []
        self.plaintext = ''

        begin = 0
        if not self.as_hex:
            for end in range(8, len(self.ciphertext) + 1, 8):
                block = self.ciphertext[begin:end]
                binary_block = self.create_binary_block(block)
                self.compute_initial_permutation(binary_block)
                self.compute_pc1()

                for round_number in range(1, 17):
                    self.left_sides.append(self.right_sides[-1])
                    self.right_sides.append(
                        self.left_sides[-2] ^ self.mangling_function(
                            self.right_sides[-1], self.keys[16 - round_number]
                        )
                    )

                begin = end
                self.plaintext += self._get_ciphertext()
                # print(self.plaintext)
        else:
            for end in range(16, len(self.ciphertext) + 1, 16):
                block = self.ciphertext[begin:end]
                binary_block = convert_str_to_binary(block, self.as_hex)
                self.compute_initial_permutation(binary_block)
                self.compute_pc1()

                for round_number in range(1, 17):
                    self.left_sides.append(self.right_sides[-1])
                    self.right_sides.append(
                        self.left_sides[-2] ^ self.mangling_function(
                            self.right_sides[-1], self.keys[16 - round_number]
                        )
                    )

                begin = end
                self.plaintext += self._get_ciphertext()
                # print(self.plaintext)

        return self.plaintext.upper()

    def encrypt(self):
        begin = 0

        if not self.as_hex:
            for end in range(8, len(self.plaintext) + 1, 8):
                block = self.plaintext[begin:end]
                binary_block = convert_str_to_binary(block, self.as_hex)
                self.compute_initial_permutation(binary_block)
                self.compute_pc1()

                for round_number in range(1, 17):
                    self.left_sides.append(self.right_sides[-1])
                    self.right_sides.append(
                        self.left_sides[-2] ^ self.mangling_function(
                            self.right_sides[-1],
                            self.compute_pc2(round_number)
                        )
                    )

                begin = end
                self.ciphertext += self._get_ciphertext()
        else:
            for end in range(16, len(self.plaintext) + 1, 16):
                block = self.plaintext[begin:end]
                binary_block = convert_str_to_binary(block, self.as_hex)
                self.compute_initial_permutation(binary_block)
                self.compute_pc1()

                for round_number in range(1, 17):
                    self.left_sides.append(self.right_sides[-1])
                    self.right_sides.append(
                        self.left_sides[-2] ^ self.mangling_function(
                            self.right_sides[-1],
                            self.compute_pc2(round_number)
                        )
                    )
                    # print(f'{round_number:2}: LEFT = {self.left_sides[round_number]} |'
                    #       f'RIGHT = {self.right_sides[round_number]} | KEY = {self.keys[round_number - 1]}')

                begin = end
                self.ciphertext += self._get_ciphertext()

        return self.ciphertext.upper()

    def _get_ciphertext(self):
        block = self.right_sides[-1] + self.left_sides[-1]
        # block = self.left_sides[-1] + self.right_sides[-1]
        bits = self.permute(block, INVERSE_INITIAL_PERMUTATION)

        begin = 0
        chars = []
        for end in range(8, len(bits) + 1, 8):
            chars.append(ba2hex(bits[begin:end]))
            begin = end

        return ''.join(chars)

    def mangling_function(self, right_side, key):
        expanded_bitstring = self.permute(right_side, EXPANSION_FUNCTION)
        xor = expanded_bitstring ^ key

        b_list = []
        begin = 0
        for end in range(6, len(xor) + 1, 6):
            b_list.append(xor[begin:end])
            begin = end

        c_of_j_list = []
        for i, b in enumerate(b_list):
            row = bitarray(2)
            row[0] = b[0]
            row[1] = b[-1]
            row_num = ba2int(row)

            column = bitarray(4)
            for j in range(1, 5):
                column[j - 1] = b[j]
            col_num = ba2int(column)

            c_of_j_list.append(int2ba(S_BOXES[i][row_num * 16 + col_num], 4))

        big_c = bitarray()
        for i in c_of_j_list:
            big_c += i
        # print(f'S-BOX outputs = {big_c}')
        return self.permute(big_c, PERMUTATION)

    def compute_initial_permutation(self, block):
        first_permutation = self.permute(block,
                                         INITIAL_PERMUTATION)
        self.left_sides.append(first_permutation[:32])
        self.right_sides.append(first_permutation[32:])

    def compute_pc1(self):
        permutation = self.permute(self.bin_key, PC1)
        self.c_list.append(permutation[:28])
        self.d_list.append(permutation[28:])

    def compute_pc2(self, round_number: int):
        self.c_list.append(self.left_cyclic_shift(self.c_list[-1],
                                                  round_number))
        self.d_list.append(self.left_cyclic_shift(self.d_list[-1],
                                                  round_number))
        key_of_round = self.permute(self.c_list[round_number] +
                                    self.d_list[round_number], PC2)
        # print(f'Key of round: {key_of_round}')
        self.keys.append(key_of_round)
        return key_of_round

    @staticmethod
    def left_cyclic_shift(what: bitarray, round_number: int):
        permutation = bitarray()

        if round_number in (1, 2, 9, 16):
            for i in range(1, len(what)):
                permutation.append(what[i])
            permutation.append(what[0])
        else:
            for i in range(2, len(what)):
                permutation.append(what[i])
            permutation.append(what[0])
            permutation.append(what[1])

        return permutation

    @staticmethod
    def permute(block, permutation):
        x0 = bitarray()
        for position in permutation:
            x0.append(block[position - 1])

        return x0
