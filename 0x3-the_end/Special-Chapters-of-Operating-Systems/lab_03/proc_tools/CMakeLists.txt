cmake_minimum_required(VERSION 3.20)
project(proc_tools C)

set(CMAKE_C_STANDARD 11)

add_executable(proc_tools main.c lib/constants.h src/helpers.c lib/helpers.h src/commands.c lib/commands.h
        src/constants.c)
add_executable(processor auxiliary/processor.c src/helpers.c lib/helpers.h lib/constants.h src/constants.c)
add_executable(consumer auxiliary/consumer.c src/helpers.c lib/helpers.h lib/constants.h src/constants.c)
