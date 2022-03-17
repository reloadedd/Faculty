cmake_minimum_required(VERSION 3.20)
project(Permissionless C)

set(CMAKE_C_STANDARD 11)

add_executable(Permissionless main.c src/funcutils.c include/funcutils.h src/permutils.c include/permutils.h include/constants.h)
add_executable(P1 src/P1.c include/P1.h src/funcutils.c include/funcutils.h src/permutils.c include/permutils.h include/constants.h)
add_executable(P2 src/P2.c include/P2.h src/funcutils.c include/funcutils.h src/permutils.c include/permutils.h include/constants.h)
add_executable(P3 src/P3.c include/P3.h src/funcutils.c include/funcutils.h src/permutils.c include/permutils.h include/constants.h)
