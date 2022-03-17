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

/*--=== INCLUDES ===--*/
#include "include/cgm.h"


/*--=== GLOBALS ===--*/
DWORD G_KILL_SWITCH_COUNT   = 0;
DWORD G_NUMBER_OF_ROADS     = 0;

/*--=== FUNCTION DECLARATIONS ===--*/
void sigKill(LPHANDLE processHandlesList, DWORD size);
void closeHandles(LPHANDLE handleArray, DWORD size);


/*--=== ENTRY POINT ===--*/
int _tmain(int argc, TCHAR *argv[]) {
    if (argc != 2) {
        handleError("CCM needs the number of roads as commandline argument");
    }

    G_NUMBER_OF_ROADS = (DWORD) strtoul(argv[1], NULL, 10);
    if (G_NUMBER_OF_ROADS == 0) {
        handleError("The number of roads is 0. This may be because of you providing 0 or a string (???)");
    }

    _tprintf(TEXT("[CCM]\tPowered on.\n"));
    LPDWORD trainMovements = createDwordArrayFromFile(F_INTERSECTION_INPUT_VALUES);

    /* Create the input mmap */
    HANDLE hMemInput;
    DWORD inputMmapSize = 22 * G_NUMBER_OF_ROADS;
    if ((hMemInput = CreateFileMapping(INVALID_HANDLE_VALUE,   /* Used to create mapping not backed up by disk file */
                                       NULL,
                                       PAGE_READWRITE,
                                       0,
                                       inputMmapSize,
                                       MMAP_INPUT_CONTROLLER)) == NULL) {
        handleError("Unable to create memory mapping (@ CCM/main.c)");
    }

    LPCSTR pBufInput = MapViewOfFile(hMemInput,
                                     FILE_MAP_READ,
                                     0,
                                     0,
                                     inputMmapSize);
    if (pBufInput == NULL) {
        handleError("Unable to get the contents of the (input) memory map (@ CCM/main.c)");
    }

    /* Create the output mmap */
    HANDLE hMemOutput;
    if ((hMemOutput = CreateFileMapping(INVALID_HANDLE_VALUE,   /* Used to create mapping not backed up by disk file */
                                        NULL,
                                        PAGE_READWRITE,
                                        0,
                                        100,
                                        MMAP_OUTPUT_CONTROLLER)) == NULL) {
        handleError("Unable to create memory mapping (@ CCM/main.c)");
    }

    LPCSTR pBufOutput = MapViewOfFile(hMemOutput,
                                      FILE_MAP_WRITE,
                                      0,
                                      0,
                                      100);
    if (pBufOutput == NULL) {
        handleError("Unable to get the contents of the (output) memory map (@ CCM/main.c)");
    }

    HANDLE hBarrier;
    if ((hBarrier = CreateEvent(NULL,
                                TRUE,
                                FALSE,
                                G_BARRIER)) == NULL) {
        handleError("Unable to create barrier (@ CCM/main.c)");
    }

    LPHANDLE processes = malloc(G_NUMBER_OF_ROADS * sizeof(HANDLE));
    for (int i = 0; i < G_NUMBER_OF_ROADS; ++i) {
        STARTUPINFO info = {sizeof(info)};
        PROCESS_INFORMATION processInfo;

        TCHAR filePathWithCommandLineArgs[MAX_PATH + 22 + 1];
        snprintf(filePathWithCommandLineArgs, MAX_PATH + 11, "%s %d %lu",
                 RCM_EXE_PATH, i + 1, G_NUMBER_OF_ROADS);

        if (CreateProcess(RCM_EXE_PATH,                             /* Module name */
                          filePathWithCommandLineArgs,              /* Command line */
                          NULL,
                          NULL,
                          FALSE,
                          0,
                          NULL,
                          NULL,
                          &info,
                          &processInfo) == 0) {
            handleError("Unable to launch executable (@ CCM/main.c)");
        }

        processes[i] = processInfo.hProcess;
    }

    /* Give time to each process to create its own mutex */
    Sleep(100);
    LPHANDLE hMutexArray = malloc(G_NUMBER_OF_ROADS * sizeof(HANDLE));
    for (int i = 1; i <= G_NUMBER_OF_ROADS; ++i) {
        TCHAR mutexName[17] = {0};
        snprintf(mutexName, 17, "%d_Mutex", i);
        if ((hMutexArray[i - 1] = OpenMutex(SYNCHRONIZE,
                                            TRUE,
                                            mutexName)) == NULL) {
            handleError("Unable to open mutex (@ CCM/main.c)");
        }
    }

    int counter = 0;
    TCHAR receivedFromRCM[100];
    TCHAR delimiters[] = ":";
    while (counter < G_GENERIC_VEHICLE_COUNT) {
        ResetEvent(hBarrier);

        LPDWORD roadValues = malloc(G_NUMBER_OF_ROADS * sizeof(DWORD));
        SecureZeroMemory(roadValues, G_NUMBER_OF_ROADS * sizeof(DWORD));

        for (int i = 0; i < G_NUMBER_OF_ROADS && G_KILL_SWITCH_COUNT < KILL_SWITCH_ACTIVATION_COUNTER; ++i) {
            SecureZeroMemory(receivedFromRCM, 22 * sizeof(TCHAR));

            DWORD mutexResult;
            if ((mutexResult = WaitForSingleObject(hMutexArray[i], REAL_TIMEOUT_MILLIS)) == WAIT_OBJECT_0) {
                CopyMemory((LPVOID) receivedFromRCM, pBufInput + i * 22, 22);
                ReleaseMutex(hMutexArray[i]);
            } else if (mutexResult == WAIT_TIMEOUT) {
                G_KILL_SWITCH_COUNT++;
                ReleaseMutex(hMutexArray[i]);
                continue;
            } else {
                handleError("Error when waiting for mutex (@ CCM/main.c)");
            }

            TCHAR* context = NULL;
            TCHAR* token = strtok_s(receivedFromRCM, delimiters, &context);

            DWORD index = (DWORD) strtoul(token, NULL, 10);
            if ((token = strtok_s(NULL, delimiters, &context)) == NULL) {
                handleError("This shouldn't be NULL (@ CCM/main.c)");
            }
            DWORD value = (DWORD) strtoul(token, NULL, 10);

            printf("[CCM | count = %d] INDEX = %lu | VALUE = %lu.\n", counter, index, value);
            assert(index - 1 < G_NUMBER_OF_ROADS);
            roadValues[index - 1] = value;
        }
        /* Announce the other processes that they can now continue */
        SetEvent(hBarrier);

        /* Here we make sure that at the unit time T = $counter all the processes are synced up, otherwise
         * kill 'em all.
         * */
        if (G_KILL_SWITCH_COUNT == KILL_SWITCH_ACTIVATION_COUNTER) {
            sigKill(processes, G_NUMBER_OF_ROADS);
        }

        /* Advance to the next time unit T */
        counter++;

        free(roadValues);
    }

    /* Cleanup */
    WaitForMultipleObjects(G_NUMBER_OF_ROADS, processes, TRUE, INFINITE);
    UnmapViewOfFile(pBufInput);
    UnmapViewOfFile(pBufOutput);
    CloseHandle(hMemInput);
    CloseHandle(hMemOutput);
    CloseHandle(hBarrier);
    closeHandles(hMutexArray, G_NUMBER_OF_ROADS);
    free(processes);
    free(trainMovements);
}


/*--=== FUNCTION DEFINITIONS ===--*/
void sigKill(LPHANDLE processHandlesList, DWORD size) {
    for (int i = 0; i < size; ++i) {
        if (TerminateProcess(processHandlesList[i], EXIT_SUCCESS) == 0) {
            handleError("Unable to terminate process (commands.c @ line 57)");
        } else {
            printf("INFO\tKilled RCM instance #%d.\n", i + 1);
        }

        CloseHandle(processHandlesList[i]);
    }
}

void closeHandles(LPHANDLE handleArray, DWORD size) {
    for (int i = 0; i < size; ++i) {
        CloseHandle(handleArray[i]);
    }

    free(handleArray);
}