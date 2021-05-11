# Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>

import random
from functools import reduce


class LFSRGenerator:
    def __init__(self, bits: int):
        self.bits = bits

    def generate_initial_state(self):
        """Generate the initial state of the PRNG.

        Return a random sequence of 64 bits, converted to a numeric value.
        """
        seed = [str(random.choice([0, 1])) for _ in range(self.bits)]
        return int(''.join(seed), 2)

    def generate_bits(self, seed=1, taps=None):
        """Generate the sequence of bits."""
        if taps is None:
            yield StopIteration
        else:
            # Remember: we are counting from right!
            taps = [self.bits - tap for tap in taps]

            register = seed & (1 << self.bits) - 1
            while True:
                # Collect the output bits from the other taps to perform XOR
                tap_bits = [(register >> tap) & 1 for tap in taps]

                # Perform XOR between taps
                bit = reduce(lambda x, y: x ^ y, tap_bits)

                # Yield the rightmost bit
                yield register & 1

                # Update to the next state (or cycle if you will)
                register &= (1 << self.bits) - 1
                register = (register >> 1) | (bit << (self.bits - 1))
