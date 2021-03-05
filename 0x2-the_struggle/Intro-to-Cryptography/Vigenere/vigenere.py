#!/usr/bin/env python3

# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
import argparse
from utils import display_error, display_info
from Encryptor import Encryptor
from Decryptor import Decryptor

INPUT_FILE = None
OUTPUT_FILE = None


def main():
    global INPUT_FILE
    global OUTPUT_FILE

    args = argparse.ArgumentParser(description="Vigenère cipher decryption "
                                               "tool.")

    args.add_argument("-e", "--encrypt", help="Encrypt the input file",
                      action="store_true", dest="mode_encrypt")
    args.add_argument("-d", "--decrypt", help="Decrypt the input file",
                      action="store_true", dest="mode_decrypt")
    args.add_argument("-i", "--input-file", help="The file from which the text"
                                                 " will be read to be "
                                                 "encrypted or decrypted")
    args.add_argument("-o", "--output-file", help="The file where the output "
                                                  "will be saved")
    args.add_argument("-v", "--verbose", help="Enable verbosity",
                      action="store_true")

    parser = args.parse_args()

    if not parser.mode_encrypt and not parser.mode_decrypt:
        display_error("Mode must be set using --encrypt or --decrypt options.")

    if parser.input_file:
        INPUT_FILE = parser.input_file

        try:
            with open(INPUT_FILE, "r") as _:
                pass
        except FileNotFoundError:
            display_error(f"The file '{INPUT_FILE}' does not exist.")
        except PermissionError:
            display_error(f"You do not have enough permissions to read the "
                          f"file '{INPUT_FILE}'")
    else:
        display_error("No input file provided.")

    if parser.output_file:
        OUTPUT_FILE = parser.output_file
    else:
        display_error("No output file provided")

    if parser.mode_encrypt:
        # Creating a new Encryptor object and uploading the plaintext to be
        # encrypted with a random key
        encrypt = Encryptor(parser.verbose)
        encrypt.read_plaintext_from_file(INPUT_FILE)
        encrypt.encrypt()
        encrypt.save_ciphertext_to_file(OUTPUT_FILE)

        display_info("Encryption performed successfully.")
        display_info(f"Saved encrypted text to '{OUTPUT_FILE}'")
    else:
        # Now, prepare the decrypting process by creating a new Decryptor
        # object which will crack the key by itself
        enigma = Decryptor(parser.verbose)
        enigma.read_ciphertext_from_file(INPUT_FILE)
        decrypted_key = enigma.crack_key()
        enigma.decrypt(decrypted_key)
        enigma.save_decrypted_plaintext_to_file(OUTPUT_FILE)

        display_info("Decryption performed successfully.")
        display_info(f"Saved decrypted text to '{OUTPUT_FILE}'")


if __name__ == '__main__':
    main()
