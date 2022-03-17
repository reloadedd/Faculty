cmake_minimum_required(VERSION 3.20)
project(directoryHierarchy C)

set(CMAKE_C_STANDARD 99)

add_executable(directoryHierarchy main.c src/stage1.c lib/stage1.h lib/headers.h src/stage2.c lib/stage2.h src/stage3.c lib/stage3.h)
