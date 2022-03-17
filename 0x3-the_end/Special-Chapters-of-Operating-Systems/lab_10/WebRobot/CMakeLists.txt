cmake_minimum_required(VERSION 3.20)
project(WebRobot C)

set(CMAKE_C_STANDARD 11)

add_executable(WebRobot main.c lib/constants.h src/utils.c lib/utils.h)
