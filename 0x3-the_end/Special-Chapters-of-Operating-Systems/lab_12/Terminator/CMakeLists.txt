cmake_minimum_required(VERSION 3.20)
project(Terminator C)

set(CMAKE_C_STANDARD 11)

add_executable(P1 src/P1.c lib/P1.h src/helpers.c lib/helpers.h)
add_executable(P2 src/P2.c lib/P2.h src/helpers.c lib/helpers.h)
