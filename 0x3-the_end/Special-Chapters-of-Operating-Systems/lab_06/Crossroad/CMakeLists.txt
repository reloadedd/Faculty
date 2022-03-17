cmake_minimum_required(VERSION 3.20)
project(Crossroad C)

set(CMAKE_C_STANDARD 11)

add_executable(CCM CCM/main.c include/constants.h CCM/include/cgm.h include/helpers.h src/helpers.c)
add_executable(RCM RCM/main.c include/constants.h RCM/include/rcm.h include/helpers.h src/helpers.c)
