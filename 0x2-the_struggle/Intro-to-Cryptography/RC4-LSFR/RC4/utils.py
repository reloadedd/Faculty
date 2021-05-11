# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

from random import randint
from string import printable, whitespace


def generate_random_sequence(length: int) -> list:
    """Generates a random printable sequence.

    :param length:  the length of the resulting sequence.
    :return:        the sequence, as a list of characters.
    """
    # All printable characters, excluding whitespaces
    chars = list(set(printable) - set(whitespace))
    sequence = []

    for i in range(length):
        pick = randint(0, len(chars)) - 1  # Pick a random character
        sequence += chars[pick]

    return sequence
