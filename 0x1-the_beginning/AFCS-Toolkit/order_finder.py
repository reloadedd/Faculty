#!/usr/bin/env python3

import sys


def find_order(a, m):
    """Find the order of a modulo m."""
    k = 1
    result = a ** k % m
    print(f'{a} ^ {k} ≡ {result} mod {m}')

    while result != 1:
        k += 1
        result = a ** k % m
        print(f'{a} ^ {k} ≡ {result} mod {m}')
    print(f'-->\t The order of {a} modulo {m} is {k}')


def print_usage():
    print('#' * 80)
    print('Find the order of a modulo m')
    print('[*] Formula: ordm(a) = min{k >= 1 | a^k ≡ 1 mod m}\n')
    print(f'Usage: python3 {sys.argv[0]} a m')
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

    find_order(a, m)


if __name__ == '__main__':
    main()
