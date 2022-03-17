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
#include <windows.h>

#include "../lib/helpers.h"


int main(int argc, const char *argv[]) {
    printf("[%ld]\targc: %d | argv[1]: %s\n", GetCurrentProcessId(), argc, argv[1]);
    fflush(stdout);

    HANDLE fileHandle;
    if ((fileHandle = CreateFile(argv[1],
                                 GENERIC_READ,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL)) == INVALID_HANDLE_VALUE) {
        handle_error("Unable to open file for reading (consumer.c @ line 41)");
    }

    /* Read from file */
    DWORD size;
    if ((size = GetFileSize(fileHandle, NULL)) == INVALID_FILE_SIZE) {
        handle_error("Unable to get file size (consumer.c @ line 47)");
    }

    CHAR *buffer = malloc((size + 1) * sizeof(CHAR));
    DWORD bytes_read;
    if (!ReadFile(fileHandle, buffer, size, &bytes_read, NULL)) {
        handle_error("Unable to read from file (consumer.c @ line 53)");
    }
    buffer[bytes_read] = '\0';

    DWORD lowercaseCharactersCount = 0;
    DWORD uppercaseCharactersCount = 0;
    DWORD numbersCount = 0;

    DWORD index = 0;
    CHAR currentChar;
    while ((currentChar = buffer[index])) {
        if (currentChar >= 'a' && currentChar <= 'z') {
            lowercaseCharactersCount++;
        } else if (currentChar >= 'A' && currentChar <= 'Z'){
            uppercaseCharactersCount++;
        } else if (currentChar >= '0' && currentChar <= '9') {
            numbersCount++;
        }

        index++;
    }

    printf("[%ld]\tLOWERCASE: %ld | UPPERCASE: %ld | NUMBERS: %ld\n", GetCurrentProcessId(),
           lowercaseCharactersCount, uppercaseCharactersCount, numbersCount);

    CloseHandle(fileHandle);
    free(buffer);
}