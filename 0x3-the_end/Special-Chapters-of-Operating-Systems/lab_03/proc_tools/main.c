/*********************************************************************************
 *          Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/constants.h"
#include "lib/helpers.h"
#include "lib/commands.h"


int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage: & \"%s\" <options> (in Powershell)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], C_CMDLINE_SWITCH_LIST_PROCESSES) == 0) {
        list_processes();
    } else if (strcmp(argv[1], C_CMDLINE_SWITCH_KILL_PROCESS) == 0) {
        if (argc < 3) {
            printf("ERROR\t%s option requires a PID.\n", C_CMDLINE_SWITCH_KILL_PROCESS);
            exit(EXIT_FAILURE);
        }

        DWORD convertedProcessID = parse_process_id(argv[2]);
        printf("INFO\tTerminating process having PID %lu...\n", convertedProcessID);
        fflush(stdout);

        terminate_process(convertedProcessID);
    } else if (strcmp(argv[1], C_CMDLINE_SWITCH_LIST_MODULES) == 0) {
        if (argc < 3) {
            printf("ERROR\t%s option requires a PID.\n", C_CMDLINE_SWITCH_LIST_MODULES);
            exit(EXIT_FAILURE);
        }

        DWORD convertedProcessID = parse_process_id(argv[2]);
        printf("INFO\tListing modules for process having PID %lu...\n", convertedProcessID);
        fflush(stdout);

        list_modules_for_process(convertedProcessID);
    } else if (strcmp(argv[1], C_CMDLINE_SWITCH_PAUSE_PROCESS) == 0) {
        if (argc < 3) {
            printf("ERROR\t%s option requires a PID.\n", C_CMDLINE_SWITCH_PAUSE_PROCESS);
            exit(EXIT_FAILURE);
        }

        DWORD convertedProcessID = parse_process_id(argv[2]);
        printf("INFO\tPausing all threads for process having PID %lu...\n", convertedProcessID);
        fflush(stdout);

        pause_all_threads_for_process(convertedProcessID);
    } else if (strcmp(argv[1], C_CMDLINE_SWITCH_LAUNCH_EXE) == 0) {
        if (argc < 3) {
            printf("ERROR\t%s option requires a path to an executable.\n", C_CMDLINE_SWITCH_LAUNCH_EXE);
            exit(EXIT_FAILURE);
        }

        launch_executable(argv[2]);
    } else if (strcmp(argv[1], C_CMDLINE_SWITCH_MEMORY_MAP) == 0) {
        if (argc < 3) {
            printf("ERROR\t%s option requires a an input and a output file.\n", C_CMDLINE_SWITCH_MEMORY_MAP);
            exit(EXIT_FAILURE);
        }

        STARTUPINFO info = {sizeof(info)};
        PROCESS_INFORMATION processInfo;

        /* Guess what this number it's gonna be */
        size_t size = ((1 << (16 - 1)) - 1);
        LPSTR cmdline = malloc(size * sizeof(CHAR));
        ZeroMemory(cmdline, size);

        strcpy_s(cmdline, size, CONSUMER_EXE_PATH);
        strcat_s(cmdline, size, " ");
        strcat_s(cmdline, size, argv[2]);

        printf("INFO\tLaunching '%s'...\n", CONSUMER_EXE_PATH);
        fflush(stdout);
        if (CreateProcess(CONSUMER_EXE_PATH,                /* No module name (use from command line) */
                          cmdline,                          /* Command line */
                          NULL,
                          NULL,
                          FALSE,
                          0,
                          NULL,
                          NULL,
                          &info,
                          &processInfo) == 0) {
            handle_error("Unable to launch executable (main.c @ line 87)");
        }

        /* Wait until the child process terminated */
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        printf("INFO\tExecutable launched successfully.\n");

        ZeroMemory(cmdline, size);
        strcpy_s(cmdline, size, PROCESSOR_EXE_PATH);
        strcat_s(cmdline, size, " ");
        strcat_s(cmdline, size, argv[3]);

        printf("INFO\tLaunching '%s'...\n", PROCESSOR_EXE_PATH);
        fflush(stdout);
        if (CreateProcess(PROCESSOR_EXE_PATH,                /* No module name (use from command line) */
                          cmdline,                          /* Command line */
                          NULL,
                          NULL,
                          FALSE,
                          0,
                          NULL,
                          NULL,
                          &info,
                          &processInfo) == 0) {
            handle_error("Unable to launch executable (main.c @ line 87)");
        }

        /* Wait until the child process terminated */
        WaitForSingleObject(processInfo.hProcess, INFINITE);
        printf("INFO\tExecutable launched successfully.\n");
    } else if (strcmp(argv[1], C_CMDLINE_HELP) == 0) {
        display_help_menu();
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "ERROR\tWrong arguments provided. Re-run using --help for more information.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}