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

#ifndef TERMINATOR_P1_H
#define TERMINATOR_P1_H

/**********************
 * --=== INCLUDES ===--
 *********************/
#include <stdio.h>
#include <windows.h>
#include <sysinfoapi.h>

#include "../lib/helpers.h"


/*********************
 * --=== DEFINES ===--
 ********************/
#define OUT_FILE    __TEXT("info.txt")


/***********************************
 * --=== FUNCTION DECLARATIONS ===--
 **********************************/
extern void getProcessorInfo(void);
extern DWORD CountSetBits(ULONG_PTR bitMask);
extern void getNUMAInfo(void);
extern void getCPUSetsInfo(void);

#endif //TERMINATOR_P1_H
