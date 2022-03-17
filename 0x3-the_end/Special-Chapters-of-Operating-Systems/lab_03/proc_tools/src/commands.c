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

#include "../lib/commands.h"


void list_processes(void) {
    HANDLE hProcess;
    PROCESSENTRY32 pe32;

    if ((hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) {
        handle_error("Unable to create handle (commands.c @ line 31)");
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcess, &pe32)) {
        handle_error("Unable to obtain the first process (commands.c @ line 37)");
    }

    /* Prepare the structure for the table */
    printf("--===[ PID ]===--\t--===[ PPID ]===--\t--===[ EXECUTABLE PATH ]===--\n");

    /* Iterate over all processes */
    do {
        printf("\t%ld\t\t\t%ld\t\t\t%s\n", pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
    } while (Process32Next(hProcess, &pe32));

    CloseHandle(hProcess);
}

void terminate_process(DWORD processID) {
    HANDLE hProcess;

    if ((hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID)) == INVALID_HANDLE_VALUE) {
        handle_error("Unable to open process (commands.c @ line 53)");
    }

    if (TerminateProcess(hProcess, EXIT_SUCCESS) == 0) {
        handle_error("Unable to terminate process (commands.c @ line 57)");
    } else {
        printf("INFO\tProcess with PID '%ld' has been successfully terminated.\n", processID);
    }

    CloseHandle(hProcess);
}

void list_modules_for_process(DWORD processID) {
    HANDLE hModule;
    MODULEENTRY32 me32;

    if ((hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID)) == INVALID_HANDLE_VALUE) {
        handle_error("Unable to create handle (commands.c @ line 67)");
    }

    me32.dwSize = sizeof(MODULEENTRY32);
    if (!Module32First(hModule, &me32)) {
        handle_error("Unable to obtain the first module (commands.c @ line 72)");
    }

    /* Prepare the structure for the table */
    printf("\t--===[ MODULE NAME ]===--\t--===[ MODULE EXECUTABLE PATH ]===--\n");

    /* Iterate over all processes */
    do {
        printf("\t%20s\t\t%s\n", me32.szModule, me32.szExePath);
    } while (Module32Next(hModule, &me32));

    CloseHandle(hModule);
}

void pause_all_threads_for_process(DWORD processID) {
    HANDLE hThread;
    THREADENTRY32 te32;

    if ((hThread = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)) == INVALID_HANDLE_VALUE) {
        handle_error("Unable to create handle (commands.c @ line 89)");
    }

    te32.dwSize = sizeof(THREADENTRY32);
    if (!Thread32First(hThread, &te32)) {
        handle_error("Unable to obtain the first thread (commands.c @ line 94)");
    }

    /* Iterate over all processes */
    do {
        if (te32.th32OwnerProcessID == processID) {
            printf("INFO\tSuspending execution of thread having TID %ld...\n", te32.th32ThreadID);
            fflush(stdout);

            HANDLE hCurrentThread;
            if ((hCurrentThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID)) == NULL) {
                handle_error("Unable to open handle to thread (commands.c @ line 104");
            }

            DWORD suspendedCount;
            if ((suspendedCount = SuspendThread(hCurrentThread)) == (DWORD) -1) {
                handle_error("Unable to suspend thread (commands.c @ line 110)");
            } else if (suspendedCount > 0) {
                /* This means that the thread was already suspended, so we're going to resume it
                 * Note that we use a loop going +1, this is because the suspendedCount is the *previous* count
                 * of suspended threads. So, by calling SuspendThread() now, that number is going to increment.
                 */
                for (int i = 0; i < suspendedCount + 1; ++i) {
                    if (ResumeThread(hCurrentThread) == (DWORD) -1) {
                        handle_error("Unable to resume thread (commands.c @ line 115)");
                    }
                }
            }

            CloseHandle(hCurrentThread);
        }
    } while (Thread32Next(hThread, &te32));

    CloseHandle(hThread);
}

void launch_executable(LPSTR path) {
    STARTUPINFO info = {sizeof(info)};
    PROCESS_INFORMATION processInfo;

    printf("INFO\tLaunching '%s'...\n", path);
    fflush(stdout);

    if (CreateProcess(NULL,             /* No module name (use from command line) */
                      path,             /* Command line */
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

    printf("INFO\tSetting 'SE_DEBUG_NAME' privilege to the newly created process...\n");
    fflush(stdout);

    HANDLE accessToken;
    if (OpenProcessToken(processInfo.hProcess,
                         TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                         &accessToken)) {
        SetPrivilege(accessToken, SE_DEBUG_NAME, TRUE);
        CloseHandle(accessToken);
    } else {
        handle_error("Unable to open process token (commands.c @ line 161)");
    }

    /* Wait until the child process terminated */
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    printf("INFO\tExecutable launched successfully.\n");

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}
