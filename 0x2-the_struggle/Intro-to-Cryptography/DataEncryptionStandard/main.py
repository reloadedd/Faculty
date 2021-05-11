#!/usr/bin/env python3

# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
from DES import Algorithm
from DES.utils import *
from rich import print


def main():
    plaintext = 'Send nuke to USA. Kim Jon Un, out.'
    plaintext_as_hex = '0123456789ABCDEF'
    key_as_hex = '133457799BBCDFF1'

    encryptor = Algorithm.DESEncryptor(plaintext_as_hex, key_as_hex, True)
    # encryptor = Algorithm.DESEncryptor(convert_to_hex_from_str(plaintext), as_hex=True)
    # encryptor = Algorithm.DESEncryptor(convert_to_hex_from_str(plaintext), 'abcdef0102030405', as_hex=True)
    print(f'[blue][ INFO ]:[/blue] Encrypted: {encryptor.encrypt()}')
    print(f'[blue][ INFO ]:[/blue] Decrypted: {encryptor.decrypt()}')
    # print(f'[blue][ INFO ]:[/blue] Decrypted: {convert_to_str_from_hex(encryptor.decrypt())}')


if __name__ == '__main__':
    main()
