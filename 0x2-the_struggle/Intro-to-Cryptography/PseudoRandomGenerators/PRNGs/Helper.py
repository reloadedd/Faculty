# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

from random import getrandbits, randrange
from PRNGs.config import MAX_INTEGER_BITS
from pathlib import Path
from rich import print


class Helper:
    @staticmethod
    def generate_big_number() -> int:
        """Generate a big number stored on the number of bits specified.

        :returns    that big number.
        """
        bits = getrandbits(MAX_INTEGER_BITS)

        return bits

    @staticmethod
    def is_prime(number: int, passes: int = 128) -> bool:
        """Check if a number is prime using the Millen-Rabin algorithm
         repeatedly.

        :param number:  the number to be checked.
        :param passes:  the number of tests to do.
        :returns        a boolean value which indicates if the number is prime.

        @Credits: https://medium.com/@prudywsh/how-to-generate-big-prime-numbers-miller-rabin-49e6e6af32fb
        """
        if number in (2, 3):
            return True

        if number <= 1 or number % 2 == 0:
            return False

        # find r and s
        s = 0
        r = number - 1
        while r & 1 == 0:
            s += 1
            r //= 2

        # Do `passes` tests
        for _ in range(passes):
            a = randrange(2, number - 1)
            x = pow(a, r, number)
            if x != 1 and x != number - 1:
                j = 1
                while j < s and x != number - 1:
                    x = pow(x, 2, number)
                    if x == 1:
                        return False
                    j += 1
                if x != number - 1:
                    return False

        return True

    @staticmethod
    def generate_prime_candidate() -> int:
        """Generate an odd integer randomly

        :returns    the number why *may* be a prime number
        """
        prime_candidate = getrandbits(MAX_INTEGER_BITS)

        # Apply a mask to set MSB and LSB to 1
        prime_candidate |= (1 << MAX_INTEGER_BITS - 1) | 1
        return prime_candidate

    @staticmethod
    def generate_big_prime() -> int:
        """ Generate a (big) prime number.

        :returns    that big prime number.
        """
        while True:
            potential_prime = Helper.generate_prime_candidate()
            if Helper.is_prime(potential_prime, 128):
                break

        return potential_prime

    @staticmethod
    def generate_big_m() -> int:
        """Generate a big number that is the product of two primes.

        :returns    that big number.
        """
        p = Helper.generate_big_prime()
        q = Helper.generate_big_prime()
        m = p * q
        print(f'[green][ DEBUG ]:[/green] P = {p}')
        print(f'[green][ DEBUG ]:[/green] Q = {q}')
        print(f'[green][ DEBUG ]:[/green] M = {m}')

        return m

    @staticmethod
    def print_setup_info() -> None:
        """Prints some information regarding the setup."""
        print(f'[red][ INFO ]:[/red] Using {MAX_INTEGER_BITS}-bit numbers.')

    @staticmethod
    def get_file_size(file: str):
        """Return the size of a file, in bytes.

        :param file     the name of the file.
        :returns        the size of the file.
        """
        return Path(file).stat().st_size
