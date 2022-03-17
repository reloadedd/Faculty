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
#include "include/rcm.h"


/*--=== GLOBALS ===--*/
DWORD G_NUMBER_OF_ROADS = 0;
DWORD G_ROAD_NUM        = 0;


/*--=== ENTRY POINT ===--*/
int _tmain(int argc, TCHAR *argv[]) {
    if (argc != 3) {
        handleError("RCM needs the road number and total number of roads as commandline arguments!");
    }

    TCHAR inputDiskFile[MAX_PATH + 1];
    snprintf(inputDiskFile, MAX_PATH, "%s\\input_drum_%s.txt", D_IO_FOLDER_REL_PATH, argv[1]);
    G_ROAD_NUM          = (DWORD) strtoul(argv[1], NULL, 10);
    G_NUMBER_OF_ROADS   = (DWORD) strtoul(argv[2], NULL, 10);
    DWORD inputMmapSize = 22 * G_NUMBER_OF_ROADS;

    _tprintf(TEXT("[RCM]\tSpawned instance #%s.\n"), argv[1]);
    fflush(stdout);

    /* Create the input mmap */
    HANDLE hMemInput;
    if ((hMemInput = OpenFileMapping(FILE_MAP_WRITE,
                                     TRUE,
                                     MMAP_INPUT_CONTROLLER)) == NULL) {
        handleError("Unable to open memory mapping for writing (@ RCM/main.c)");
    }

    LPCSTR pBufInput = MapViewOfFile(hMemInput,
                                     FILE_MAP_WRITE,
                                     0,
                                     0,
                                     inputMmapSize);
    if (pBufInput == NULL) {
        handleError("Unable to get the contents of the (input) memory map (@ RCM/main.c)");
    }

    /* Create the output mmap */
    HANDLE hMemOutput;
    if ((hMemOutput = OpenFileMapping(FILE_MAP_READ,
                                      TRUE,
                                      MMAP_OUTPUT_CONTROLLER)) == NULL) {
        handleError("Unable to open memory mapping for reading (@ RCM/main.c)");
    }

    LPCSTR pBufOutput = MapViewOfFile(hMemOutput,
                                      FILE_MAP_READ,
                                      0,
                                      0,
                                      100);
    if (pBufOutput == NULL) {
        handleError("Unable to get the contents of the (output) memory map (@ RCM/main.c)");
    }

    HANDLE hMutex;
    HANDLE hBarrier;
    TCHAR mutexName[17] = {0};
    snprintf(mutexName, 17, "%lu_Mutex", G_ROAD_NUM);
    if ((hMutex = CreateMutex(NULL,
                              TRUE,
                              mutexName)) == NULL) {
        handleError("Unable to create mutex (@ CCM/main.c)");
    }
    if ((hBarrier = OpenEvent(EVENT_ALL_ACCESS,
                              TRUE,
                              G_BARRIER)) == NULL) {
        handleError("Unable to open barrier object (@ RCM/main.c)");
    }

    LPDWORD vehicleMovements = createDwordArrayFromFile(inputDiskFile);
    TCHAR sendToCCM[22];
    for (int i = 0; i < G_GENERIC_VEHICLE_COUNT; ++i) {
        /* 22 = <FIRST NUMBER>:<SECOND NUMBER>\0 */
        SecureZeroMemory(sendToCCM, 22 * sizeof(TCHAR));
        SecureZeroMemory((LPVOID) (pBufInput + (G_ROAD_NUM - 1) * 22), 22 * sizeof(TCHAR));
        snprintf(sendToCCM, 22, "%s:%lu", argv[1], vehicleMovements[i]);

        if (WaitForSingleObject(hMutex, INFINITE) != WAIT_FAILED) {
            CopyMemory((LPVOID) (pBufInput + (G_ROAD_NUM - 1) * 22), sendToCCM, (_tcslen(sendToCCM) + 1) * sizeof(TCHAR));
            if (ReleaseMutex(hMutex) == 0) {
                handleError("Unable to release the mutex (@ RCM/main.c)");
            }
        } else {
            handleError("Error when waiting for mutex (@ RCM/main.c)");
        }

        if (WaitForSingleObject(hBarrier, INFINITE) != WAIT_FAILED) {
//            _tprintf(TEXT("instance #%s\tAM TRECUT DE BARIERA\n"), argv[1]);
            continue;   /* Do nothing but get to the next iteration. This point serves as a sync point */
        } else {
            handleError("Wait failed for barrier (@ RCM/main.c)");
        }
    }

    /* Cleanup */
    UnmapViewOfFile(pBufInput);
    UnmapViewOfFile(pBufOutput);
    CloseHandle(hMemInput);
    CloseHandle(hMemOutput);
    CloseHandle(hMutex);
    CloseHandle(hBarrier);
    free(vehicleMovements);
}