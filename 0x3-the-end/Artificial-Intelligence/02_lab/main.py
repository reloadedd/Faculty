#!/usr/bin/env python3

from strategies import Strategy
from state import *


def menu():
    print('- Powered on.')
    number_of_couples = int(input('INFO\tEnter the number of couples >>> '))
    couples = {i: i + number_of_couples for i in
               range(1, number_of_couples + 1)}
    print(f'INFO\tThe pair of couples: {couples}')

    print('For the following question, separate the values using commas.')
    print('NOTE\tLeave empty if everybody is at the initial shore.')
    answer = input('Which persons are at the final shore? ')

    shore = [False for _ in range(number_of_couples * 2)]
    try:
        for person in answer.split(','):
            shore[int(person.strip()) - 1] = True
    except ValueError:
        pass

    print(f'\t--- {shore}')

    print('NOTE\tDefault: the boat is at the initial shore.')
    answer = input('Where is the boat [initial/final] ? ')
    if not answer:
        boat = INITIAL_SHORE
    else:
        while True:
            if "initial" in answer:
                boat = INITIAL_SHORE
                break
            elif "final" in answer:
                boat = FINAL_SHORE
                break
            else:
                print('ERROR\tWrong answer. Accepted answers: [initial/final].'
                      ' Try again!')
                answer = input('Where is the boat [initial/final] ? ')
    print(f'\t--- '
          f'{"INITIAL_SHORE" if boat == INITIAL_SHORE else "FINAL_SHORE"}')

    state = State(shore, boat)
    print('Great! Now choose the strategy you want to apply on the instance:')
    print('\n---== BEGIN Menu ==---')
    print('\t1. Backtracking')
    print('\t2. BFS')
    print('\t3. Hill Climbing')
    print('\t4. A*')
    print('\t5. Quit menu')
    print('---== END Menu ==---\n')

    option = int(input('>>> '))
    while option < 1 or option > 5:
        print('ERROR\tYou must choose between 1-5!')
        option = int(input('>>> '))

    if option == 1:
        Strategy(state).backtracking(state)
    elif option == 2:
        Strategy(state).bfs()
    elif option == 3:
        Strategy(state).hill_climbing()
    elif option == 4:
        Strategy(state).a_star()
    elif option == 5:
        print('Bye')
        exit(1)


def test_run():
    number_of_couples = int(input('INFO\tEnter the number of couples >>> '))

    couples = {i: i + number_of_couples for i in
               range(1, number_of_couples + 1)}
    print(f'INFO\tThe pair of couples: {couples}')

    initial_state = create_initial_state(number_of_couples * 2)

    print(initial_state.shore)
    print(f'The transition is valid? '
          f'{validate_transition(initial_state, 1, 3, FORWARD)}')
    next_state = transition(initial_state, 8, 6, FORWARD)
    print(next_state.shore)


def main():
    # test_run()
    menu()


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        exit(1)
