#!/usr/bin/env python3

import sys
from simplify_mod_m import simplify


def solve_equation(a, p):
    """Find the solution (if it has) for equations of the form: x^2 ≡ a mod p."""
    if simplify(a, p) != a:
        a = simplify(a, p)
        print(f'[INFO] The simplified equation is now: x^2 ≡ {a} mod {p}')

    for x in range(p):
        result = simplify(x ** 2, p)
        print(f'[DEBUG] Tried x = {x:3}, which is {x ** 2:4} mod {p}\t--> simplified: {result:3} mod {p}')
        if result == a:
            return x    # This is the solution
    return -1   # The equation doesn't have any solution


def print_usage():
    print('#' * 80)
    print('Find the solution (if it has) for equations of the form: x^2 ≡ a mod p\n')
    print(f'Usage: python3 {sys.argv[0]} a p')
    exit(0)


def main():
    if len(sys.argv) != 3:
        print_usage()
    try:
        a = int(sys.argv[1])
        p = int(sys.argv[2])
    except ValueError:
        print(f'The arguments must be all integers!')
        print_usage()

    if p < 1:
        print('The remainder classes modulo m start at m >= 1!')
        print_usage()

    result = solve_equation(a, p)
    if result != -1:    # a is a quadratic residue mod p, because it has a solution
        print(f'The solution of the equation x^2 ≡ {a} mod {p} is:\n-->\tx0 = {result}')
    else:   # a is NOT a quadratic residue mod p
        print('The equation doesn\'t have a solution\nExiting...')


if __name__ == '__main__':
    main()
