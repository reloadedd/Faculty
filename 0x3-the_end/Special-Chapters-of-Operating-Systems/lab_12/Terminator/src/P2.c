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

/**********************
 * --=== INCLUDES ===--
 *********************/
#include "../lib/P2.h"


/******************************
 * --=== GLOBAL VARIABLES ===--
 *****************************/
enum { G_HOW_MANY_PAIRS = 100 };


/*************************
 * --=== MAIN DRIVER ===--
 ************************/
int _tmain() {
    /* Initialize the PRNG - Yeah, I see the warning, but I'm not trying to create a blockchain here
     * nor a military-grade secure system. I'm a simple man, just trying to generate some arrays.
     */
    srand(time(NULL));

    _tprintf(_T("INFO\tCreating the necessary folders...\n"));
    createAndSetWorkingDirectory(OUT_FOLDER);
    createAllFolders();

    _tprintf(_T("INFO\tGenerating the pairs...\n"));
    DWORD K = 5;    /* Increase it on your own risk. You have been warned! */
    DWORD**** pairsForEachN = generatePairsForEachN(K);
//    printPairs(pairsForEachN, K);

    _tprintf(_T("INFO\tSaving the pairs on disk...\n"));
    saveBigArrayOnDisk(pairsForEachN, K);

    _tprintf(_T("INFO\tFreeing the big array...\n"));
    freeWholeThing(pairsForEachN, K);
}


/**********************************
 * --=== FUNCTION DEFINITIONS ===--
 *********************************/
void createAllFolders(void) {
    if (CreateDirectory(DATA_FOLDER, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }

    if (CreateDirectory(RESULTS_FOLDER, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }

    if (CreateDirectory(RESULTS_SEQUENTIAL_FOLDER, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }

    if (CreateDirectory(RESULTS_STATIC_FOLDER, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }

    if (CreateDirectory(RESULTS_DYNAMIC_FOLDER, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }

    if (CreateDirectory(RESULTS_STATIC_RESTRICTED_FOLDER, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }

    if (CreateDirectory(RESULTS_DYNAMIC_RESTRICTED_FOLDER, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }
}

void printPairs(DWORD**** pairsForEachN, DWORD k) {
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < G_HOW_MANY_PAIRS; ++j) {
            for (int m = 0; m < (ULONG) pow(10, i + 1); ++m) {
                _tprintf(_T("pairsForEachN[%d][%d][0][%d] = %ld\n"), i + 1, j, m, pairsForEachN[i][j][0][m]);
                _tprintf(_T("pairsForEachN[%d][%d][1][%d] = %ld\n"), i + 1, j, m, pairsForEachN[i][j][1][m]);
            }
        }
    }
}

void saveBigArrayOnDisk(DWORD**** array, DWORD k) {
    for (DWORD i = 0; i < k; ++i) {
        ULONG N = (ULONG) pow(10, i + 1);

        for (DWORD j = 0; j < G_HOW_MANY_PAIRS; ++j) {
            for (DWORD m = 0; m < 2; ++m) {
                TCHAR path[MAX_PATH];
                HANDLE hFile;

                snprintf(path, MAX_PATH, _T("%s/%ld_%ld_%c.txt"), DATA_FOLDER, i + 1, j, (m == 0 ? 'A' : 'B'));
                if((hFile = CreateFile(path,
                                       GENERIC_WRITE,
                                       0,
                                       NULL,
                                       CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_NORMAL,
                                       NULL)) == INVALID_HANDLE_VALUE) {
                    handleError(__MESSAGE("Unable to create writable handle for file"));
                }

                DWORD maxSize = N * 11 + 1; /* A DWORD can have maximum 10 digits + 1 for newline and 1 for NULL */
                TCHAR *output = malloc(maxSize);
                ZeroMemory(output, maxSize);

                for (DWORD elementIndex = 0; elementIndex < N; ++elementIndex) {
                    snprintf(output + strlen(output),
                             maxSize - strlen(output),
                             _T("%lu\n"),
                             array[i][j][m][elementIndex]);
                }

                if (!WriteFile(hFile,
                               output,
                               strlen(output),
                               (void *) 0x0,
                               NULL)) {
                    handleError(__MESSAGE("Unable to write to file"));
                }
            }
        }
    }
}

DWORD**** generatePairsForEachN(DWORD k) {
    DWORD**** skyIsTheLimit = malloc(k * sizeof(DWORD ***));
    ULONG N = (ULONG) pow(10, k);

    for (DWORD i = 0; i < k; ++i) {
        skyIsTheLimit[i] = generateArrayPairs(N);
    }

    return skyIsTheLimit;
}

DWORD*** generateArrayPairs(ULONG size) {
    PDWORD** pairs = malloc(G_HOW_MANY_PAIRS * sizeof(PDWORD *));

    for (DWORD i = 0; i < G_HOW_MANY_PAIRS; ++i) {
        pairs[i] = malloc(2 * sizeof(PDWORD));
    }

    for (DWORD i = 0; i < G_HOW_MANY_PAIRS; ++i) {
        pairs[i][0] = generateRandomArray(size);
        pairs[i][1] = generateRandomArray(size);
    }

    return pairs;
}

PDWORD generateRandomArray(ULONG size) {
    PDWORD randomArray = malloc(size * sizeof(DWORD));

    for (ULONG i = 0; i < size; ++i) randomArray[i] = rand();   // NOLINT(cert-msc30-c, cert-msc50-cpp)

    return randomArray;
}

void freeWholeThing(DWORD**** thing, DWORD k) {
    for (DWORD firstDimension = 0; firstDimension < k; ++firstDimension) {
        for (DWORD secondDimension = 0; secondDimension < G_HOW_MANY_PAIRS; ++secondDimension) {
            for (DWORD thirdDimension = 0; thirdDimension < 2; ++thirdDimension) {
                free(thing[firstDimension][secondDimension][thirdDimension]);
            }
        }
    }

    for (DWORD firstDimension = 0; firstDimension < k; ++firstDimension) {
        for (DWORD secondDimension = 0; secondDimension < G_HOW_MANY_PAIRS; ++secondDimension) {
            free(thing[firstDimension][secondDimension]);
        }
    }

    for (DWORD firstDimension = 0; firstDimension < k; ++firstDimension) {
        free(thing[firstDimension]);
    }

    free(thing);
}
