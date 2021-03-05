# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
import string
from rich import print

ENGLISH_ALPHABET = string.ascii_uppercase
ENGLISH_ALPHABET_LENGTH = 26
ENGLISH_LETTERS_FREQUENCIES = {
    'A': 0.08167,
    'B': 0.01492,
    'C': 0.02782,
    'D': 0.04253,
    'E': 0.12702,
    'F': 0.02228,
    'G': 0.02015,
    'H': 0.06094,
    'I': 0.06966,
    'J': 0.00153,
    'K': 0.00772,
    'L': 0.04025,
    'M': 0.02406,
    'N': 0.06749,
    'O': 0.07507,
    'P': 0.01929,
    'Q': 0.00095,
    'R': 0.05987,
    'S': 0.06327,
    'T': 0.09056,
    'U': 0.02758,
    'V': 0.00978,
    'W': 0.02360,
    'X': 0.00150,
    'Y': 0.01974,
    'Z': 0.00074
}


def display_error(error_message):
    """Display an error message and exit.

    @param error_message: the error message to be displayed
    """
    print(f"[bold red][ ERROR ]:[/bold red] {error_message}")
    exit(-1)


def display_verbose(message):
    """Display verbose output.

    @param message: the message to be displayed"""
    print(f"[bold magenta][ VERBOSE ]:[/bold magenta] {message}")


def display_info(message):
    """Display an informational message.

    @param message: the info message to be displayed
    """
    print(f"[bold cyan][ INFO ]:[/bold cyan] {message}")
