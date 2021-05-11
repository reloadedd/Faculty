# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

from rich import print
from itertools import permutations


class RC4Encryptor:
    """Encryptor class for RC4 Stream Cipher."""
    MAX_BYTE = 256

    def __init__(self, key: str, plaintext: str) -> None:
        self.key = self._check_key(key)
        if not self.key:
            print('[red]error:[/red] the length of the key must be >=5 and '
                  '<=16!')
            exit(-1)

        self.key_length = len(self.key)
        self.current_state = None
        self.plaintext = plaintext
        self.ciphertext = None

    @staticmethod
    def _check_key(key: str) -> str:
        """Checks if the key has the required length."""
        if not (5 <= len(key) <= 16):
            return ''
        return key

    def init_stage(self) -> None:
        """The Key-Scheduling Algorithm (KSA) used for RC4."""
        j = 0
        s0 = list(permutations(range(RC4Encryptor.MAX_BYTE)).__next__())

        for i in range(RC4Encryptor.MAX_BYTE):
            j = (j + s0[i] + ord(self.key[i % self.key_length])) % \
                RC4Encryptor.MAX_BYTE
            s0[i], s0[j] = s0[j], s0[i]

        self.current_state = 0, 0, s0

    def trans_stage(self) -> int:
        """The Pseudo-Random Generation Algorithm (PRGA) used for RC4.

        :return: an encrypted byte corresponding to the current stage.
        """
        i, j, current_s = self.current_state
        i = (i + 1) % RC4Encryptor.MAX_BYTE
        j = (j + current_s[i]) % RC4Encryptor.MAX_BYTE

        current_s[i], current_s[j] = current_s[j], current_s[i]

        # Update the current state with the new state we've just reached
        self.current_state = i, j, current_s

        # Return the current encrypted byte
        return current_s[(current_s[i] + current_s[j]) % RC4Encryptor.MAX_BYTE]

    def encrypt(self) -> None:
        """Encrypts the plaintext."""
        self.init_stage()

        self.ciphertext = []
        for symbol in self.plaintext:
            self.ciphertext.append(ord(symbol) ^ self.trans_stage())

    def get_str_ciphertext(self) -> str:
        """Returns the ciphertext as a (not very likely) readable string.

        :return: the ciphertext as string.
        """
        if not self.ciphertext:
            self.encrypt()
        return ''.join(chr(i) for i in self.ciphertext)

    def get_hex_ciphertext(self) -> str:
        """Returns the ciphertext as an hex string.

        :return: the plaintext as hex string.
        """
        if not self.ciphertext:
            self.encrypt()
        return ''.join(f'{i:0{2}x}' for i in self.ciphertext)
