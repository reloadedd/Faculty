/*  Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>. Some rights reversed. */
#ifndef DIRECTORYHIERARCHY_STAGE1_H
#define DIRECTORYHIERARCHY_STAGE1_H

#include "headers.h"
#include "stage2.h"

extern int append_to_path(LPSTR path, LPCSTR to_be_appended);
extern void trigger_stage_one_protocol(LPSTR bridge);

#endif //DIRECTORYHIERARCHY_STAGE1_H
