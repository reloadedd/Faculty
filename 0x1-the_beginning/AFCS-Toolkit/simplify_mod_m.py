#!/usr/bin/env python3

import sys


def simplify(a, m):
    """Computes the reminder modulo m.
    If a number belongs to a class of numbers modulo m, then that number will be less than m
    For instance, 2 modulo 3 is in its final form, because it's less than m (which is 3), but if you have, for example,
    202 mod 3, then this is equivalent to: 1 mod 3
    """
    if a >= m:
        while a >= m:
            a %= m
    return a


def print_usage():
    print('#' * 80)
    print(f'Usage: python3 {sys.argv[0]} a m')
    print('e.g: for 202 mod 3')
    print(f'you will run: python3 {sys.argv[0]} 202 3')
    exit(0)


def main():
    if len(sys.argv) != 3:
        print_usage()
    try:
        a = int(sys.argv[1])
        m = int(sys.argv[2])
    except ValueError:
        print(f'The arguments must be all integers!')
        print_usage()

    if m < 1:
        print('The remainder classes modulo m start at m >= 1!')
        print_usage()

    print(f'Got input: {a} mod {m}')
    a = simplify(a, m)
    print(f'The equivalent simplified expression is:\n-->\t{a} mod {m}')


if __name__ == '__main__':
    main()
