#!/usr/bin/env python3
# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

from RC4.RC4Encryptor import RC4Encryptor
from RC4.RC4Decryptor import RC4Decryptor
from LFSR.LFSRGenerator import LFSRGenerator
from rich.console import Console
from LFSR.config import *

# ---== Global Objects ==---
CONSOLE = Console()
RC4_PASSWORD = 'whiterabbit'
RC4_PLAINTEXT = 'This is Kim. Send nuke to USA.'


def rc4():
    CONSOLE.print("---== RC4 Encryption & Decryption ==---", style="bold red")
    encryptor = RC4Encryptor(RC4_PASSWORD, RC4_PLAINTEXT)
    CONSOLE.print(f'Encrypted (str) message: {encryptor.get_str_ciphertext()}')
    CONSOLE.print(f'Encrypted (hex) message: {encryptor.get_hex_ciphertext()}')

    ciphertext = encryptor.get_str_ciphertext()
    decryptor = RC4Decryptor(RC4_PASSWORD, ciphertext)
    CONSOLE.print(f'Decrypted (str) message: {decryptor.get_str_plaintext()}')
    CONSOLE.print(f'Decrypted (hex) message: {decryptor.get_hex_plaintext()}')


def lfsr():
    CONSOLE.print("---== LFSR Pseudo-Random Generator ==---", style="bold red")
    generator = LFSRGenerator(BITS)
    ones = 0
    zeroes = 0
    j = 0

    CONSOLE.print(f'Using {BITS}-bit LFSR.')
    for i in generator.generate_bits(seed=generator.generate_initial_state(),
                                     taps=[64, 4, 3, 1]):
        ones += 1 if i == 1 else 0
        zeroes += 1 if i == 0 else 0
        j += 1
        if j == MAX_ITERATIONS:
            break

    CONSOLE.print(f'Pseudo-random bits generated: {MAX_ITERATIONS}')

    ones_percentage = ones / MAX_ITERATIONS * 100
    zeroes_percentage = zeroes / MAX_ITERATIONS * 100
    CONSOLE.print(f'Distribution of ones: {ones_percentage}%')
    CONSOLE.print(f'Distribution of zeroes: {zeroes_percentage}%')


def main():
    # RC4 Encryption & Decryption
    rc4()

    # LFSR Pseudo-Random Generator
    lfsr()


if __name__ == '__main__':
    main()
