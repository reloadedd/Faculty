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
#include "../include/funcutils.h"


/*--=== FUNCTION DEFINITIONS ===--*/
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

void launchExecutable(LPCSTR path) {
    /*
     * Description
     * -----------
     *  Launch the executable specified by the `path` argument and wait for it to finish.
     */
    STARTUPINFO info = {sizeof(info)};
    PROCESS_INFORMATION processInfo;

    if (CreateProcess(path,         /* Module name */
                      NULL,         /* Command line */
                      NULL,
                      NULL,
                      FALSE,
                      0,
                      NULL,
                      NULL,
                      &info,
                      &processInfo) == 0) {
        handleError(__MESSAGE_PARAMS("Unable to launch P1"));
    }

    /* Make sure to wait for it to finish */
    WaitForSingleObject(processInfo.hProcess, INFINITE);
}

void logSuccessOrFailure(LPCSTR message, HANDLE hFile) {
    /*
     * Description
     * -----------
     *  Log the status of the function called to the given handle.
     */
    DWORD dwBytesWritten = 0;
    if (WriteFile(hFile,
                  message,
                  strlen((const char *) message),
                  &dwBytesWritten,
                  NULL) == FALSE) {
        handleError(__MESSAGE_PARAMS("Unable to write the SID to file"));
    }
}