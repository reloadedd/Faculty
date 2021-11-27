#!/usr/bin/env python3

import pyaes
from . import config
from .logging import LOGGER


class CustomAES:
    """Implement AES in ECB and CTR modes"""
    def __init__(self, key: str, mode: str, iv: bytearray = None) -> None:
        """Initialization.

        :param key:     the key used to encrypt the message.
        :param mode:    the mode used to encrypt [ECB|CTR].
        :param iv:      optional. The IV used for encryption in CTR mode.
        """
        self.key = self.__add_padding(key)[0]
        self.iv = iv
        self.mode = mode

        if mode == config.MODE_XXX and not iv:
            LOGGER.error("We also need an IV if you want to use CTR mode.")
            exit(-1)

        self.aes = pyaes.AES(self.key)

    @staticmethod
    def __add_padding(message: str) -> list:
        """Adds padding to a block or create a dummy padding block.

        This function implement the PKCS#7 padding algorithm. Basically, there
        is always padding, and the padding represents the number of bytes added
        and each byte is the number of bytes added. For example:
            - Suppose the padding is 3 (0x3 in hex), then the padding will be:
                => \x03\x03\x03
            - Suppose the padding is 11 (0xb in hex), then the padding will be:
                => \x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b

        :param message:     the string to be padded.
        :returns:           a list with the padded block(s).
        """
        block_bytes = bytearray(message, encoding="utf-8")
        padded = []
        for block in range(0, len(block_bytes), config.AES_BLOCK_SIZE):
            padded.append(block_bytes[block:block + config.AES_BLOCK_SIZE])

        if not padded:
            return [""]

        if len(padded[len(padded) - 1]) % config.AES_BLOCK_SIZE:
            how_much_padding = config.AES_BLOCK_SIZE - \
                               len(padded[len(padded) - 1])
            padded[len(padded) - 1].extend(
                [how_much_padding] * (
                        config.AES_BLOCK_SIZE - len(padded[len(padded) - 1])
                )
            )
        else:
            # Even though the block fits perfectly, we will add a dummy block
            how_much_padding = config.AES_BLOCK_SIZE
            padded.append([how_much_padding] * config.AES_BLOCK_SIZE)
        return padded

    @staticmethod
    def __remove_padding(blocks: bytearray) -> bytearray:
        """Remove padding from a block generated using PKCS#7 algorithm.

        :param blocks:  a sequence of bytes representing one or more blocks.
        :returns:       the sequence of bytes un-padded.
        """
        index = len(blocks) - 2
        position = 0

        while index >= 0:
            if blocks[index] != blocks[index + 1]:
                position = index + 1
                break
            index -= 1
        return blocks[:position]

    def encrypt(self, message: str) -> bytearray:
        """Encrypt a string using AES in ECB or CTR mode.

        :param message: the string to be encrypted.
        :returns:       the encrypted sequence of bytes.
        """
        padded_message = self.__add_padding(message)
        encrypted = bytearray()

        if self.mode == config.MODE_ECB:
            for i, block in enumerate(padded_message):
                if block:
                    encrypted.extend(self.aes.encrypt(block))
        elif self.mode == config.MODE_XXX:
            for counter, block in enumerate(padded_message):
                # Store the IV in the upper 64 bits
                nonce = self.iv.copy()
                if block:
                    # Store the counter in the lower 64 bits
                    nonce.extend(
                        bytearray("{0:08b}".format(counter).encode('utf-8'))
                    )

                    # Perform XOR between the encrypted nonce and the plaintext
                    encrypted.extend(bytearray(a ^ b for (a, b) in zip(
                        self.aes.encrypt(nonce[:16]),  block
                    )))
        return encrypted

    def decrypt(self, message: bytearray) -> bytearray:
        """Decrypt a string using AES in ECB or CTR mode.

        :param message: the string to be encrypted.
        :returns:       the decrypted sequence of bytes.
        """
        decrypted = bytearray()

        if self.mode == config.MODE_ECB:
            for block in range(0, len(message), config.AES_BLOCK_SIZE):
                decrypted.extend(self.aes.decrypt(
                    message[block:block + config.AES_BLOCK_SIZE])
                )
        elif self.mode == config.MODE_XXX:
            # Store the IV in the upper 64 bits
            counter = 0

            for block in range(0, len(message), config.AES_BLOCK_SIZE):
                # Store the counter in the lower 64 bits
                nonce = self.iv.copy()
                nonce.extend(
                    bytearray("{0:08b}".format(counter).encode('utf-8'))
                )

                decrypted.extend(bytearray(a ^ b for (a, b) in zip(
                    self.aes.encrypt(nonce[:16]),
                    message[block:block + config.AES_BLOCK_SIZE]
                )))
                counter += 1

        return self.__remove_padding(decrypted)
