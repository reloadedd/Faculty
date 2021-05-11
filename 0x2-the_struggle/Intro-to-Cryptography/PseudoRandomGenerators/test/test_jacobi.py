# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

from PRNGs import Jacobi
from PRNGs.config import HOW_MANY_NUMBERS_TO_GENERATE
from rich import print
import unittest


class TestJacobi(unittest.TestCase):
    def test_number_of_digits_consistent(self):
        """Test if the number of zeroes is balanced with the number of ones."""
        generator = Jacobi.Jacobi()
        print(f'[red][ INFO ]:[/red] Testing Jacobi PRNG.')
        result = tuple(generator.generate(HOW_MANY_NUMBERS_TO_GENERATE))

        ones = tuple(filter(lambda x: x == 1, result))
        percent = round(len(ones) / len(result) * 100, 3)
        print(f'[red][ INFO ]:[/red] The PRNG generated {len(ones)} '
              f'({percent}%) ones.')

        self.assertGreaterEqual(len(ones), int(HOW_MANY_NUMBERS_TO_GENERATE *
                                               0.45))

        zeroes = tuple(filter(lambda x: x == 0, result))
        percent = round(len(zeroes) / len(result) * 100, 2)
        print(f'[red][ INFO ]:[/red] The PRNG generated {len(zeroes)} '
              f'({percent}%) zeroes.')

        self.assertGreaterEqual(len(zeroes), int(HOW_MANY_NUMBERS_TO_GENERATE *
                                                 0.45))


def run():
    unittest.main()
