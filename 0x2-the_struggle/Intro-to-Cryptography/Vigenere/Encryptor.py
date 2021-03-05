# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
import random
from utils import ENGLISH_ALPHABET, ENGLISH_ALPHABET_LENGTH, display_verbose
from rich.progress import track


class Encryptor:
    """Vigenère cipher encryptor class."""
    def __init__(self, verbose: bool = False):
        self.plaintext = None
        self.ciphertext = None
        self.maximum_key_length = None
        self.verbose = verbose

    def __generate_random_key(self) -> str:
        """Generate a random key of maximum ENGLISH_ALPHABET_LENGTH characters.

        :returns: the key
        """
        assert self.plaintext is not None, "No plaintext given before " \
                                           "generating an encryption key"
        self.maximum_key_length = min(
            len(self.plaintext), ENGLISH_ALPHABET_LENGTH
        )

        key_length = random.randint(2, self.maximum_key_length)
        key = [''] * key_length
        for i in range(key_length):
            key[i] = random.choice(ENGLISH_ALPHABET)

        printable_key = ''.join(key)
        if self.verbose:
            display_verbose(f"Generated random key [green]{printable_key}"
                            f"[/green] of length [yellow]{len(key)}[/yellow]")

        return printable_key

    def read_plaintext_from_file(self, filename: str) -> None:
        """Read the plaintext from a file stored on the disk.

        :param filename: the name of the file
        """
        try:
            with open(filename, "r") as plaintext_file:
                self.plaintext = plaintext_file.read().upper()
        except FileNotFoundError:
            print("-- [ERROR]: '{filename}' does not exist.")
        except PermissionError:
            print("-- [ERROR]: You don't have permission to read the file "
                  "'{filename}'.")

    def save_ciphertext_to_file(self, filename: str) -> None:
        """Save the encrypted text to a file stored on the disk.

        :param filename: the name of the file
        """
        with open(filename, "w") as encrypted:
            encrypted.write(self.ciphertext)

    def encrypt(self) -> None:
        """Perform the encryption using the generated key."""
        key = self.__generate_random_key()
        self.ciphertext = [''] * len(self.plaintext)

        # A separate index for key -- only incremented if the current character
        # in the plaintext is ASCII uppercase.
        key_index = 0
        for i in track(range(len(self.plaintext)),
                       description="⌛️ Encrypting..."):
            # What we're doing here: Ci = (Pi + Ki) mod 26
            # Where:
            #   -- Ci - ciphertext at position i
            #   -- Pi - plaintext at position i
            #   -- Ki - key value at position i
            if self.plaintext[i] in ENGLISH_ALPHABET:
                self.ciphertext[i] = chr(
                    (ord(self.plaintext[i]) + ord(key[key_index % len(key)]))
                    % ENGLISH_ALPHABET_LENGTH + 65
                )
                key_index += 1
            else:
                # We won't do nothing here. If we are here, this means that
                # a foreign character was found in the plaintext, so we
                # DONT increment the key's index.
                pass
        self.ciphertext = ''.join(self.ciphertext)
