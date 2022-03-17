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
#include "../include/P3.h"


/*--=== GLOBALS ===--*/
HANDLE hFile;


int _tmain(int argc, TCHAR *argv[]) {
    if ((hFile = CreateFile(LOGS,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL)) == INVALID_HANDLE_VALUE) {
        handleError(__MESSAGE_PARAMS("Unable to open file for reading"));
    }

    /* Write the current group SID to 'P3.txt' inside the 'RESTRICTED WRITE' folder */
    writeCurrentGroupSidToFile();

    /* Write the las 6 digits of the current group's SID to the registry sub key */
    createRegistrySubKey();

    CloseHandle(hFile);
}


/*--=== FUNCTION DEFINITIONS ===--*/
void writeCurrentGroupSidToFile(void) {
    HANDLE handle;
    if ((handle = CreateFile(F_P3_TXT,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL)) == INVALID_HANDLE_VALUE) {
        handleError(__MESSAGE_PARAMS("Unable to open file for reading"));
    }

    PSID builtinUsers = malloc(SECURITY_MAX_SID_SIZE);
    DWORD sidSize = SECURITY_MAX_SID_SIZE;

    if (CreateWellKnownSid(WinBuiltinUsersSid, NULL, builtinUsers, &sidSize) == FALSE) {
        handleError(__MESSAGE_PARAMS("Unable to create well known SID for everyone"));
    }

    LPCSTR currentSidString = convertBinarySidToStringSid(builtinUsers);

    DWORD lastSixDigits = 0;
    DWORD power = 1;
    DWORD stop = strlen(currentSidString) - 6;

    for (DWORD i = strlen(currentSidString) - 1; i >= stop; --i) {
        if (currentSidString[i] != '-') {
            lastSixDigits += (currentSidString[i] - '0') * power;
        } else {
            stop--;
            continue;
        }
        power *= 10;
    }

    LPSTR lastSixDigitsString = malloc(7);
    snprintf(lastSixDigitsString, 7, "%ld", lastSixDigits);

    _tprintf(_T("[INFO: %s]\tWriting the current group SID to '%s'\n"), PREFIX, F_P3_TXT);
    DWORD dwBytesWritten = 0;
    if (WriteFile(handle,
                  currentSidString,
                  strlen(currentSidString),
                  &dwBytesWritten,
                  NULL) == FALSE) {
        logSuccessOrFailure("Failed to write the current group SID to the file.\n", hFile);
        handleError(__MESSAGE_PARAMS("Unable to write the SID to file"));
    } else {
        logSuccessOrFailure("Successfully wrote the current group SID to the file.\n", hFile);
    }

    CloseHandle(handle);
    free(lastSixDigitsString);
}

void createRegistrySubKey(void) {
    HKEY hkSub = NULL;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     REG_KEY_PATH,
                     0,
                     KEY_WRITE,
                     &hkSub) != ERROR_SUCCESS) {
        logSuccessOrFailure("Failed to open the registry sub key.\n", hFile);
        handleError(__MESSAGE_PARAMS("Unable to open registry key"));
    } else {
        logSuccessOrFailure("Successfully opened the registry key.\n", hFile);
    }

    PSID builtinUsers = malloc(SECURITY_MAX_SID_SIZE);
    DWORD sidSize = SECURITY_MAX_SID_SIZE;

    if (CreateWellKnownSid(WinBuiltinUsersSid, NULL, builtinUsers, &sidSize) == FALSE) {
        handleError(__MESSAGE_PARAMS("Unable to create well known SID for everyone"));
    }

    LPCSTR currentSidString = convertBinarySidToStringSid(builtinUsers);
    _tprintf(_T("[INFO: %s]\tBUILTIN\\Users group SID: %s\n"), PREFIX, currentSidString);

    DWORD lastSixDigits = 0;
    DWORD power = 1;
    DWORD stop = strlen(currentSidString) - 6;

    for (DWORD i = strlen(currentSidString) - 1; i >= stop; --i) {
        if (currentSidString[i] != '-') {
            lastSixDigits += (currentSidString[i] - '0') * power;
        } else {
            stop--;
            continue;
        }
        power *= 10;
    }

    _tprintf(_T("[INFO: %s]\tWriting the last 6 digits of the current group SID, skipping '-'s -> %ld\n"),
             PREFIX, lastSixDigits);
    if (RegSetValueEx(hkSub,
                      REG_SUB_KEY_NAME,
                      0,
                      REG_DWORD,
                      (const byte*) &lastSixDigits,
                      sizeof lastSixDigits)) {
        logSuccessOrFailure("Failed to write the last 6 digits of the current group SID to the registry "
                            "sub key.\n", hFile);
        handleError(__MESSAGE_PARAMS("Unable to create registry sub key"));
    } else {
        logSuccessOrFailure("Successfully wrote the last 6 digits of the current group SID to the registry "
                            "sub key.\n", hFile);
    }
}
