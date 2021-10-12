#!/usr/bin/env python3

INITIAL_SHORE = 0
FINAL_SHORE = 1
FORWARD = 2
BACKWARDS = 3
NUMBER_OF_COUPLES = -1


class State:
    """Denote a particular state of the problem"""

    def __init__(self, shore: list, boat: int) -> None:
        # People at the final shore
        self.shore = shore

        self.wives = [wife for wife in range(len(shore), len(shore) // 2, -1)]
        self.husbands = [husband for husband in range(1, len(shore) // 2 + 1)]

        # On which shore the boat belongs in this moment of time
        self.position_of_boat = boat

    def update(self, person: int, boat: int):
        self.shore[person - 1] = \
            True if boat == FINAL_SHORE else False
        self.position_of_boat = boat

    def get_husband(self, wife: int):
        return wife - len(self.shore) // 2

    def get_wife(self, husband: int):
        return husband + len(self.shore) // 2

    def check_wife_is_with_husband(self, wife: int, passenger: int):
        if self.shore[wife - 1] != \
                self.shore[self.get_husband(passenger) - 1] and \
                any([self.shore[i] == self.shore[wife - 1]
                     for i in range(NUMBER_OF_COUPLES)]):
            return False
        return True


def check_final_state(state: State) -> bool:
    if all(state.shore) \
            and state.position_of_boat == FINAL_SHORE:
        return True
    return False


def create_initial_state(number_of_couples: int) -> State:
    return State([False for _ in range(number_of_couples)], INITIAL_SHORE)


def validate_transition(state: State, driver: int, passenger: int,
                        transition_type: int) -> bool:
    if transition_type == FORWARD:
        # If the boat is already on the other shore
        if state.position_of_boat == FINAL_SHORE:
            return False
        # If the driver is already on the other shore
        if state.shore[driver - 1]:
            return False
        # If the passenger is already on the other shore
        if state.shore[passenger - 1]:
            return False
        # If a wife is on the final shore without her husband and with
        # other men
        if passenger in state.wives and \
                state.check_wife_is_with_husband(passenger, driver):
            return False
        if driver in state.wives and \
                state.check_wife_is_with_husband(driver, passenger):
            return False
        return True
    else:
        # If the boat is already on the other shore
        if state.position_of_boat == INITIAL_SHORE:
            return False
        # If the driver is already on the other shore
        if not state.shore[driver - 1]:
            return False
        # If the passenger is already on the other shore
        if not state.shore[passenger - 1]:
            return False
        # If a wife is on the final shore without her husband and with
        # other men
        if passenger in state.wives and \
                state.check_wife_is_with_husband(passenger, driver):
            return False
        if driver in state.wives and \
                state.check_wife_is_with_husband(driver, passenger):
            return False
        return True


def transition(state: State, driver: int, passenger: int,
               transition_type: int) -> State:
    if transition_type == FORWARD:
        state.update(passenger, FINAL_SHORE)
        state.update(driver, FINAL_SHORE)
    else:
        state.update(passenger, INITIAL_SHORE)
        state.update(driver, INITIAL_SHORE)

    return State(state.shore.copy(), state.position_of_boat)


def main():
    global NUMBER_OF_COUPLES
    NUMBER_OF_COUPLES = int(input('INFO\tEnter the number of couples >>> '))

    couples = {i: i + NUMBER_OF_COUPLES for i in
               range(1, NUMBER_OF_COUPLES + 1)}
    print(f'INFO\tThe pair of couples: {couples}')

    initial_state = create_initial_state(NUMBER_OF_COUPLES * 2)

    print(initial_state.shore)
    print(f'E ok tranzitia? {validate_transition(initial_state, 1, 3, FORWARD)}')
    next_state = transition(initial_state, 8, 6, FORWARD)
    print(next_state.shore)


if __name__ == '__main__':
    main()
