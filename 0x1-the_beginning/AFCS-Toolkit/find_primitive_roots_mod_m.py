#!/usr/bin/env python3

import sys


def find_order(a, m, totient):
    """Find the order of a modulo m and then check if it's equal to the totient."""
    k = 1
    result = a ** k % m
    # print(f'{a} ^ {k} ≡ {result} mod {m}')

    while result != 1 and k <= totient:
        k += 1
        result = a ** k % m
        # print(f'{a} ^ {k} ≡ {result} mod {m}')
    if k != totient:
        return -1
    return totient


def print_usage():
    print('#' * 80)
    print('Find the primitive roots modulo m\n')
    print(f'Usage: python3 {sys.argv[0]} m φ(m) φ(φ(m))')
    exit(0)


def main():
    if len(sys.argv) != 4:
        print_usage()

    try:
        m = int(sys.argv[1])
        totient = int(sys.argv[2])
        totient2 = int(sys.argv[3])
    except ValueError:
        print(f'The arguments must be all integers!')
        print_usage()

    print(f'The number of primitive roots modulo {m} is {totient2}')

    a = 1
    k = 0
    roots = []
    while k != totient2:
        if find_order(a, m, totient) != -1:
            print(f'\t => the order of {a:3} modulo {m} is {totient}')
            k += 1
            roots.append(a)
        a += 1
    print(f'The primitive roots modulo {m} are:\n-->\t{roots}')


if __name__ == '__main__':
    main()
