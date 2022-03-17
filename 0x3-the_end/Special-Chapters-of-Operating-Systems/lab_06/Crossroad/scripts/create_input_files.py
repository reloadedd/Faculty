import os
import random

CCM_FILE = '../cmake-build-debug/io/input_intersectie.txt'
RCM_GENERIC_FILE = '../cmake-build-debug/io/input_drum_{0}.txt'


def create_ccm() -> None:
    """Create input file for the CCM program.

    The input file consists on each line of:
        - 0, if the train is not passing by
        - 1, if the train is passing by
    """
    with open(CCM_FILE, 'w') as f:
        for i in range(99):
            f.write(f'{random.randint(0, 1)}\n')
        f.write(f'{random.randint(0, 1)}')


def create_rcm(n: int) -> None:
    """Create input files for the RCM program.

    Create `n` files of the form: input_drum_<n>.txt
    Those files contain the movement of the vehicle:
        - 0, no vehicle is passing by
        - 1, a normal priority vehicle is passing by
        - 2, a high priority vehicle is passing by

    :param n:   the number of files to create, each corresponding to a vehicle
    """
    for i in range(1, n + 1):
        with open(RCM_GENERIC_FILE.format(i), 'w') as f:
            for i in range(99):
                f.write(f'{random.randint(0, 2)}\n')
            f.write(f'{random.randint(0, 2)}')


def main():
    """The main driver of the script."""
    if not os.path.isdir(os.path.dirname(CCM_FILE)):
        os.mkdir(os.path.dirname(CCM_FILE))

    create_ccm()
    create_rcm(50)


if __name__ == '__main__':
    main()
