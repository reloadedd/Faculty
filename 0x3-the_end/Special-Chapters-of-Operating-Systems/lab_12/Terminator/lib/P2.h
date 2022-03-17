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

#ifndef TERMINATOR_P2_H
#define TERMINATOR_P2_H

/**********************
 * --=== INCLUDES ===--
 *********************/
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "helpers.h"


/*********************
 * --=== DEFINES ===--
 ********************/
#define _CRT_RAND_S     /* For rand_s() function */
#define OUT_FOLDER                              "P2"
#define DATA_FOLDER                             "date"
#define RESULTS_FOLDER                          "resultate"
#define RESULTS_SEQUENTIAL_FOLDER               "resultate/secvential"
#define RESULTS_STATIC_FOLDER                   "resultate/static"
#define RESULTS_DYNAMIC_FOLDER                  "resultate/dinamic"
#define RESULTS_STATIC_RESTRICTED_FOLDER        "resultate/static_restricted"
#define RESULTS_DYNAMIC_RESTRICTED_FOLDER       "resultate/dinamic_restricted"


/***********************************
 * --=== FUNCTION DECLARATIONS ===--
 **********************************/
extern PDWORD generateRandomArray(ULONG size);
extern DWORD*** generateArrayPairs(ULONG size);
extern DWORD**** generatePairsForEachN(DWORD k);
extern void freeWholeThing(DWORD**** thing, DWORD k);
extern void saveBigArrayOnDisk(DWORD**** array, DWORD k);
extern void printPairs(DWORD**** pairsForEachN, DWORD k);
extern void createAllFolders(void);

#endif //TERMINATOR_P2_H
