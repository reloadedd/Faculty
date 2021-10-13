#!/usr/bin/env python3

from constants import *


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
                     for i in range(len(self.shore) // 2)]):
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
    new_state = State(state.shore.copy(), state.position_of_boat)
    if transition_type == FORWARD:
        new_state.update(passenger, FINAL_SHORE)
        new_state.update(driver, FINAL_SHORE)
    else:
        new_state.update(passenger, INITIAL_SHORE)
        new_state.update(driver, INITIAL_SHORE)

    return new_state
