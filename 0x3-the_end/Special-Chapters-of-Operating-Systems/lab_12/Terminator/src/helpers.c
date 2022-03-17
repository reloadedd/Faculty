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
#include "../lib/helpers.h"


/**********************************
 * --=== FUNCTION DEFINITIONS ===--
 *********************************/
void handleError(LPCSTR error, DWORD lineNumber, LPCSTR fullFileName) {
    /*
     * Description
     * -----------
     *  Handle errors by displaying an informative error message, together with error code, filename and line number.
     */
    _ftprintf(stderr, "ERROR\t%s [code=%ld] (%s:%ld)\n", error, GetLastError(), basename(fullFileName), lineNumber);
    fflush(stderr);
    exit(EXIT_FAILURE);
}

LPCSTR basename(LPCSTR path) {
    /*
     * Description
     * -----------
     *  Get the basename part from an absolute path.
     *
     * Example
     * -------
     *  For C:\Whatever\WhateverAgain\YetWhatever\Something.c will return Something.c
     */
    char *base = strrchr(path, '\\');
    return base ? base+1 : path;
}

void createAndSetWorkingDirectory(LPCSTR directory) {
    if (CreateDirectory(directory, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError(__MESSAGE("Unable to create directory"));
        }
    }

    if (SetCurrentDirectory(directory) == 0) {
        handleError(__MESSAGE("Unable to set current directory"));
    }
}
