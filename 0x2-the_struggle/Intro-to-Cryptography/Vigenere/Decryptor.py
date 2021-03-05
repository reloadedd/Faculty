# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
from utils import *
from rich.progress import track


class Decryptor:
    """Vigenère cipher decryptor class."""
    def __init__(self, verbose: bool = False):
        self.plaintext = None
        self.ciphertext = None
        self.occurrence_table = None
        self.ciphertext_length = None
        self.verbose = verbose

    def __compute_occurrence_table(self, start: int = 0, step: int = 1) -> int:
        """Compute the occurrence table for the given subset.

        :param start: the starting position in the ciphertext
        :param step: the distance between two letters of the ciphertext
        :returns: the length of the newly-created subset of the ciphertext
        """
        assert self.ciphertext_length is not None,\
            "Ciphertext length not given"

        self.occurrence_table = {symbol: 0 for symbol in ENGLISH_ALPHABET}

        for i in range(start, self.ciphertext_length, step):
            self.occurrence_table[self.ciphertext[i]] += 1

        return (self.ciphertext_length - start) // step + 1

    def __determine_index_of_coincidence(self, start: int, step: int) -> int:
        """Determine the index of coincidence for the given subset.

        :param start: the starting position in the ciphertext
        :param step: the distance between two letters of the ciphertext
        :returns: the index of coincidence
        """
        ciphertext_length = self.__compute_occurrence_table(start, step)

        try:
            return sum(
                (self.occurrence_table[symbol] / ciphertext_length) *
                ((self.occurrence_table[symbol] - 1) /
                 (ciphertext_length - 1)) for symbol in ENGLISH_ALPHABET
            )
        except ZeroDivisionError:
            return 0

    def compute_key_length(self) -> int:
        """Compute the length of the key using the Index of Coincidence method.

        :returns: the length of the key
        """
        assert self.ciphertext is not None, "Empty string received as " \
                                            "ciphertext"

        for current_key_length in range(1, self.ciphertext_length):
            ic_list = []
            for start in range(current_key_length):
                ic = self.__determine_index_of_coincidence(start,
                                                           current_key_length)
                ic_list.append(ic)

            valid = True
            for ic in ic_list:
                if ic < 0.060:
                    valid = False

            if not valid:
                continue

            return current_key_length
        return -1

    def crack_key(self) -> str:
        """Crack the key of the encrypted text.

        :returns: the cracked key
        """
        if (key_length := self.compute_key_length()) == -1:
            display_error("Couldn't find the key because the ciphertext is "
                          "too small.")

        key = [''] * key_length

        for j in track(range(key_length), description='⚠️ Decrypting...'):
            s = -1

            while s < ENGLISH_ALPHABET_LENGTH:
                s += 1
                mic = self.__determine_mutual_index_of_coincidence(
                    j, key_length, s
                )

                if 0.058 <= mic <= 0.072:
                    break

            key[j] = chr((ENGLISH_ALPHABET_LENGTH - s) %
                         ENGLISH_ALPHABET_LENGTH + 65)

        printable_key = ''.join(key)
        if self.verbose:
            display_verbose(f'Found key length: [yellow]{key_length}[/yellow]')
            display_verbose(f'Found key: [green]{printable_key}[/green]')

        return printable_key

    @staticmethod
    def shift(array: list, times: int) -> None:
        """Rotate the elements of the array by the given parameter.
        :
        :param array: the list to be rotated
        :param times: the number of times to rotate the list's elements
        """
        for i in range(times):
            array.append(array.pop(0))

    def __determine_mutual_index_of_coincidence(self, start, step, times)\
            -> int:
        """Determine the Mutual Index of Coincidence for the given subset.

        :param start: the starting position in the ciphertext
        :param step: the distance between two letters of the ciphertext
        :param times: the number of times to rotate the elements of the
        generated subset of the ciphertext
        :returns: the mutual index of coincidence
        """
        partial_ciphertext = [
            ord(i) - 65 for i in self.ciphertext[start::step]
        ]

        for i in range(len(partial_ciphertext)):
            partial_ciphertext[i] = (partial_ciphertext[i] + times) %\
                                    ENGLISH_ALPHABET_LENGTH + 65

        shifted_occurrence_table = {symbol: 0 for symbol in ENGLISH_ALPHABET}
        for i in partial_ciphertext:
            shifted_occurrence_table[chr(i)] += 1

        return sum(
            ENGLISH_LETTERS_FREQUENCIES[symbol] *
            (shifted_occurrence_table[symbol] / len(partial_ciphertext))
            for symbol in ENGLISH_ALPHABET
        )

    def read_ciphertext_from_file(self, filename: str) -> None:
        """Read the ciphertext from a file stored on the disk.

        :param filename: the name of the file
        """
        try:
            with open(filename, "r") as ciphertext_file:
                self.ciphertext = [symbol for symbol in
                                   ciphertext_file.read().upper()
                                   if symbol in ENGLISH_ALPHABET]
                self.ciphertext_length = len(self.ciphertext)
        except FileNotFoundError:
            print("-- [ERROR]: '{filename}' does not exist.")
        except PermissionError:
            print("-- [ERROR]: You don't have permission to read the file "
                  "'{filename}'.")

    def save_decrypted_plaintext_to_file(self, filename: str) -> None:
        """Save the decrypted text to a file.

        :param filename: the name of the file
        """
        assert self.plaintext is not None,\
            "You must first decrypt the encrypted text before saving"

        with open(filename, "w") as decrypted:
            decrypted.write(self.plaintext)

    def get_decrypted_plaintext(self) -> str:
        """Get the decrypted text as a string.

        :returns: the decrypted text
        """
        assert self.plaintext is not None, \
            "You must first decrypt the encrypted text before saving"

        return self.plaintext

    def decrypt(self, key: str) -> None:
        """Perform the decryption of the ciphertext.

        :param key: the key used to decrypt the ciphertext
        """
        self.plaintext = [''] * len(self.ciphertext)

        # A separate index for key -- only incremented if the current character
        # in the plaintext is ASCII uppercase.
        key_index = 0
        for i in range(len(self.ciphertext)):
            if self.ciphertext[i] in ENGLISH_ALPHABET:
                # What we're doing here: Pi = (Ci - Ki) mod 26
                # Where:
                #   -- Pi - plaintext at position i
                #   -- Ci - ciphertext at position i
                #   -- Ki - key value at position i
                self.plaintext[i] = chr(
                    (ord(self.ciphertext[i]) - ord(key[key_index % len(key)]))
                    % ENGLISH_ALPHABET_LENGTH + 65
                )
                key_index += 1
            else:
                # We won't do nothing here. If we are here, this means that
                # a foreign character was found in the plaintext, so we
                # DONT increment the key's index.
                pass

        self.plaintext = ''.join(self.plaintext)
