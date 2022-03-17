/*  Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>. Some rights reversed. */
#include "../lib/stage1.h"

#define NO_OF_DIRECTORIES   3

LPCSTR DIRECTORY_STRUCTURE[] = {"CSSO", "Laboratoare", "Tema 1"};


int append_to_path(LPSTR path, LPCSTR to_be_appended) {
    if (strlen(path) + strlen(to_be_appended) + 1 >= MAX_PATH) {
        return -1;
    }

    /* Prevent adding a backslash at the beginning of the string */
    if (strlen(path) != 0 ) {
        size_t position = strlen(path);
        path[position] = '\\';
        path[position + 1] = (CHAR) 0;
    }
    strncat(path, to_be_appended, strlen(to_be_appended));

    return 0;
}

void trigger_stage_one_protocol(LPSTR bridge) {
    printf("INFO\t==== Stage One initiated. ====\n");
    /* Initialisation */
    LPSTR current_directory = get_cwd();

    LPSTR current_path = malloc((MAX_PATH + 1) * sizeof(CHAR));
    memset(current_path, 0, (MAX_PATH + 1) * sizeof(CHAR));
    strncpy(current_path, current_directory, strlen(current_directory) + 1);

    for (int i = 0; i < NO_OF_DIRECTORIES; ++i) {
        if (append_to_path(current_path, DIRECTORY_STRUCTURE[i]) == -1) {
            fprintf(stderr, "ERROR\tUnable to append '%s' to path. Code: %lu\n",
                    DIRECTORY_STRUCTURE[i], GetLastError());
            return;
        }

        if (check_path(current_path) == PATH_IS_DIRECTORY) {
            printf("INFO\tPath '%s' already exists. Skipping creation...\n", current_path);
        } else {
            printf("INFO\tCreating '%s'...\n", current_path);
            if (!CreateDirectory(current_path, NULL)) {
                fprintf(stderr, "ERROR\tWhen creating directory '%s'. Code: %lu\n",
                        DIRECTORY_STRUCTURE[i], GetLastError());
                return;
            }
        }
    }

    memcpy(bridge, current_path, (strlen(current_path) + 1) * sizeof(CHAR));
    free(current_path);
    free(current_directory);
}