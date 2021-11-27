#!/usr/bin/env python3
import random

FINAL_STATE = None


class State:
    def __init__(self, number_of_colors: int, number_of_balls: int,
                 how_many: int, choices=None):
        # this is n
        if choices is None:
            choices = []
        self.number_of_colors = number_of_colors

        # this is m
        self.number_of_balls = number_of_balls

        # this is k
        self.chosen_balls_number = how_many

        # what the player has chosen
        self.choices = choices

    def equal(self, state) -> bool:
        for choice in self.choices:
            if choice not in state.choices:
                return False
        return True


def create_initial_state(n: int, m: int, k: int) -> State:
    return State(n, m, k, random_ball_picker(k, m * n))


def random_ball_picker(k: int, total: int) -> list:
    chosen = []

    pool = [i for i in range(total)]
    for i in range(k):
        chosen.append(random.choice(pool))

    return chosen


def check_final_state(state: State, final_state: State) -> bool:
    return final_state.equal(state)


def main():
    n = int(input("n [Number of Colors] >>> "))
    m = int(input("m [Number of Balls of each Color] >>> "))
    k = int(input("k [How many balls to pick] >>> "))

    global FINAL_STATE
    FINAL_STATE = State(n, m, k, random_ball_picker(k, m * n))
    print(f'Choices: {FINAL_STATE.choices}')


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("Bye!")
