#!/usr/bin/env python3

import sys
from simplify_mod_m import simplify


def gcd(x, y):
    """Compute GCD = Greatest Common Divisor of x and y."""
    if x < y:
        x, y = y, x

    while y != 0:
        r = x % y
        x = y
        y = r
    return abs(x)


def is_solvable(a, b, m):
    """Check if the equation is solvable.
    The equation is solvable if GCD(a, m) | b <=> b % GCD(a, m) == 0
    """
    d = gcd(a, m)
    return b % d == 0


def find_x0(a, b, m):
    """Find x0 (an arbitrary solution) by guessing it."""
    if is_solvable(a, b, m):
        print(f'[INFO] The equation is solvable since (a, m) | b --> {gcd(a, m)} | {b}')
        if simplify(a, m) != a:
            a = simplify(a, m)
            print(f'[INFO] After simplifying a, the equation is now: {a}x ≡ {b} mod {m}')
        if simplify(b, m) != b:
            b = simplify(b, m)
            print(f'[INFO] After simplifying b, the equation is now: {a}x ≡ {b} mod {m}')
        for x in range(m):  # Goes up to m - 1
            result = simplify(a * x, m)
            # print(f'[DEBUG] Tried x = {x}, which is {a * x:4} mod {m}\t--> simplified: {result} mod {m}')
            if result == b:
                return x   # This is our x0
    else:
        print('The equation is not solvable\nExiting...')
        exit(0)


def compute_solution(a, b, m):
    """Compute all solutions.
    It combines all previously defined functions to get to the result.
    It first finds x0 by calling find_x0(...) and then computes the number of solutions which is GCD(a, m)
    """
    x0 = find_x0(a, b, m)
    solutions = gcd(a, m)   # The number of solutions
    print('[INFO] The solutions look like this: xi = (x0 + i * m / (a, m)) % m')
    print(f'[INFO] The number of solutions is {solutions}')
    print(f'x0 is {x0}')

    for i in range(1, solutions):
        sol = (x0 + i * (m // solutions)) % m
        # Verbose output (i.e: print the computations made to get the result)
        print(f'x{i} is ({x0} + {i} * ({m} / {solutions})) % {m} = {sol}')

        # Uncomment the line below if you want simplified output (i.e: only print the result, not the computations made)
        # print(f'x{i} is {sol}')


def print_usage():
    print('#' * 80)
    print(f'Usage: python3 {sys.argv[0]} a b m')
    print('e.g: for equation 35x ≡ 1 mod 3')
    print(f'you will run: python3 {sys.argv[0]} 35 1 3')
    exit(0)


def main():
    if len(sys.argv) != 4:
        print_usage()
    try:
        a = int(sys.argv[1])
        b = int(sys.argv[2])
        m = int(sys.argv[3])
    except ValueError:
        print(f'The arguments must be all integers!')
        print_usage()

    if m < 1:
        print('The remainder classes modulo m start at m >= 1!')
        print_usage()

    # Compute solutions
    compute_solution(a, b, m)


if __name__ == '__main__':
    main()
