from state import *


class Strategy:
    def __init__(self, state: State):
        self.state = state

    def backtracking(self, next_state: State):
        if check_final_state(next_state):
            print(f'INFO\tSolution was found.')
            exit(0)

        for i in range(len(next_state.shore) - 1):
            if not next_state.shore[i] and not next_state.shore[i + 1] and\
                    validate_transition(next_state, i, i + 1, FORWARD):
                candidate = transition(next_state, i, i + 1, FORWARD)
                print(f'INFO\tTrying {candidate.shore} | BOAT -> '
                      f'{"INITIAL_SHORE" if candidate.position_of_boat == INITIAL_SHORE else "FINAL_SHORE"}')

                self.backtracking(candidate)
                print('INFO\tBacktracking...')
            elif next_state.shore[i] and next_state.shore[i + 1] and \
                    validate_transition(next_state, i, i + 1, BACKWARDS):
                candidate = transition(next_state, i, i + 1, BACKWARDS)
                print(f'INFO\tTrying {candidate.shore} | BOAT -> '
                      f'{"INITIAL_SHORE" if candidate.position_of_boat == INITIAL_SHORE else "FINAL_SHORE"}')

                self.backtracking(candidate)
                print('INFO\tBacktracking...')

    @staticmethod
    def bfs():
        print('INFO\tNot implemented yet.')

    @staticmethod
    def hill_climbing():
        print('INFO\tNot implemented yet.')

    @staticmethod
    def a_star():
        print('INFO\tNot implemented yet.')
