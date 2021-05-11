# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

import random
from rich import print
from PRNGs.Helper import Helper


class Jacobi:
    def __init__(self):
        # Before doing anything else, display the setup
        Helper.print_setup_info()

        self.big_m = Helper.generate_big_m()
        self.seed = random.randrange(1, self.big_m - 1)
        print(f'[green][ DEBUG ]:[/green] Seed = {self.seed}')

        self.previous = self.seed

    def generate(self, how_many: int):
        """Generate a number of pseudo-random bits.

        The generator computes the Jacobi symbol for each pair
        (seed + index, M), where the index is the nth-number's position in the
        sequence of generated numbers. If the Jacobi symbol is -1 or 0, the
        generator will yield 0, else if the Jacobi symbol is 1, the generator
        will yield 1.

        :param how_many     the number of bits to generate.
        """
        for i in range(how_many):
            yield 0 if Jacobi.compute_jacobi_symbol(
                self.seed + i, self.big_m) in (-1, 0) else 1

    @staticmethod
    def compute_jacobi_symbol(a: int, n: int) -> int:
        """Compute the Jacobi Symbol according to the AFCS course presentation.

        :param a   first number needed to compute the symbol
        :param n   second number needed to compute the symbol
        :returns    the Jacobi Symbol
        """
        b = a % n
        c = n
        s = 1

        while b >= 2:
            while b % 4 == 0:
                b //= 4

            if b % 2 == 0:
                if c % 8 in (3, 5):
                    s = -s
                b //= 2

            if b == 1:
                break

            if b % 4 == 3 and c % 4 == 3:
                s = -s
            b, c = c % b, b

        return s * b
