/*  Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>. Some rights reversed. */
#ifndef DIRECTORYHIERARCHY_HEADERS_H
#define DIRECTORYHIERARCHY_HEADERS_H

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>

enum {
    PATH_DOES_NOT_EXIST = -1,
    PATH_IS_DIRECTORY   = 1,
    PATH_IS_REG_FILE    = 2
};

#endif //DIRECTORYHIERARCHY_HEADERS_H
