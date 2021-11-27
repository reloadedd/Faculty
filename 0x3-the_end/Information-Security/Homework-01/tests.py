#!/usr/bin/env python3

from node import config
from node.logging import LOGGER
from node.CustomAES import CustomAES

FILENAME = "sample-2mb-text-file.txt"


def main():
    LOGGER.info("Encrypting a simple text (16 bytes) with ECB mode...")
    aes = CustomAES("test_key", config.MODE_ECB)
    print(f'Encrypted: {aes.encrypt("01234567890abcde")}')
    print(f'Decrypted: {aes.decrypt(aes.encrypt("01234567890abcde"))}')

    LOGGER.info("Encrypting a big file (2M) with ECB mode...")
    aes = CustomAES("test_key", config.MODE_ECB)
    with open(FILENAME) as f:
        content = f.read()

        LOGGER.info(f"Saving the encrypted file ({FILENAME}.ECB)...")
        with open(f"{FILENAME}.ECB", "wb") as f2:
            f2.write(aes.encrypt(content))

        LOGGER.info(f"Saving the decrypted file ({FILENAME}.ECB_decrypted)...")
        with open(f"{FILENAME}.ECB_decrypted", "wb") as f3:
            f3.write(aes.decrypt(aes.encrypt(content)))

    LOGGER.info("Encrypting a simple text (16 bytes) with CTR mode...")
    aes = CustomAES("test_key", config.MODE_XXX,
                    config.AES_INITIALIZATION_VECTOR)
    print(f'Encrypted: {aes.encrypt("01234567890abcde")}')
    print(f'Decrypted: {aes.decrypt(aes.encrypt("01234567890abcde"))}')

    LOGGER.info("Encrypting a big file (2M) with ECB mode...")
    aes = CustomAES("test_key", config.MODE_XXX,
                    config.AES_INITIALIZATION_VECTOR)
    with open(FILENAME) as f:
        content = f.read()

        LOGGER.info(f"Saving the encrypted file ({FILENAME}.CTR)...")
        with open("sample-2mb-text-file.txt.CTR", "wb") as f2:
            f2.write(aes.encrypt(content))

        LOGGER.info(f"Saving the decrypted file ({FILENAME}.CTR_decrypted)...")
        with open("sample-2mb-text-file.txt.CTR_decrypted", "wb") as f3:
            f3.write(aes.decrypt(aes.encrypt(content)))


if __name__ == '__main__':
    main()
