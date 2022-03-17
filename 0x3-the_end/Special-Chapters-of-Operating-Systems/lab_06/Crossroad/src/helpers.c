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
#include "../include/helpers.h"


/*--=== FUNCTION DEFINITIONS ===--*/
void handleError(const char* error) {
    _ftprintf(stderr, "ERROR\t%s [code=%ld]\n", error, GetLastError());
    exit(EXIT_FAILURE);
}

LPDWORD createDwordArrayFromFile(const TCHAR* filename) {
    HANDLE hFile;
    if ((hFile = CreateFile(filename,
                            GENERIC_READ,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL)) == INVALID_HANDLE_VALUE) {
        handleError("Unable to open file for reading (@ helpers.c)");
    }

    DWORD size = 0;
    if ((size = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE) {
        handleError("Unable to get file size (@ helpers.c)");
    }
    TCHAR* buffer = malloc((size + 1) * sizeof(TCHAR));
    ZeroMemory(buffer, size + 1);

    if (ReadFile(hFile,
                 buffer,
                 size,
                 NULL,
                 NULL) == FALSE) {
        handleError("Unable to read from file (@ helpers.c)");
    }

    TCHAR delimiters[] = "\n";
    TCHAR* context = NULL;
    TCHAR* token = strtok_s(buffer, delimiters, &context);

    LPDWORD arr = malloc(G_GENERIC_VEHICLE_COUNT * sizeof(DWORD));
    ZeroMemory(arr, G_GENERIC_VEHICLE_COUNT);

    int i = 0;
    char* unused_ptr;
    while (token != NULL && i < G_GENERIC_VEHICLE_COUNT) {
        arr[i++] = (DWORD) strtoul(token, &unused_ptr, 10);

        token = strtok_s(NULL, delimiters, &context);
    }

    CloseHandle(hFile);
    free(buffer);

    return arr;
}
