/*  Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>. Some rights reversed. */
#ifndef DIRECTORYHIERARCHY_STAGE2_H
#define DIRECTORYHIERARCHY_STAGE2_H

#include "headers.h"
#include "stage1.h"

extern LPSTR get_cwd(void);
extern void create_files(void);
extern short int check_path(LPCSTR path);
extern void write_summary(LPCSTR base_path);
extern void trigger_stage_two_protocol(LPSTR bridge, LPSTR bridge_back);
extern void enumerate_registry_metadata(HKEY hive, LPCSTR lpFileName);

#endif //DIRECTORYHIERARCHY_STAGE2_H
