/*  Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>. Some rights reversed. */
#include "lib/stage1.h"
#include "lib/stage2.h"
#include "lib/stage3.h"


/* Main driver of the program.
 *
 * ... dramatic music intensifies ...
 */
int main() {
    LPSTR next_level = malloc((MAX_PATH + 1) * sizeof(CHAR));
    LPSTR final_level = malloc((MAX_PATH + 1) * sizeof(CHAR));
    memset(next_level, 0, (MAX_PATH + 1) * sizeof(CHAR));
    memset(final_level, 0, (MAX_PATH + 1) * sizeof(CHAR));

    /* First exercise */
    trigger_stage_one_protocol(next_level);

    if (!SetCurrentDirectory(next_level)) {
        fprintf(stderr, "ERROR\tWhen changing directory to '%s'. Code: %lu", next_level, GetLastError());
        return 1;
    }

    /* Second exercise */
    trigger_stage_two_protocol(next_level, final_level);

    /* Third exercise */
    trigger_stage_three_protocol(final_level);

    free(next_level);
    free(final_level);
    return 0;
}
