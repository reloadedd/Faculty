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
#include "../include/P2.h"


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
        handleError(__MESSAGE_PARAMS("Unable to open file for writing"));
    }

    /* Write the current SID to 'P2.txt' inside the 'RESTRICTED WRITE' folder */
    writeCurrentSidToFile();

    /* Create the registry key */
    createRegistryKey();

    _tprintf(_T("[INFO: %s]\tLaunching '%s'...\n"), PREFIX, P3_EXE_PATH);
    launchCustomExecutable();

    CloseHandle(hFile);
}


/*--=== FUNCTION DEFINITIONS ===--*/
void writeCurrentSidToFile(void) {
    HANDLE handle;
    if ((handle = CreateFile(F_P2_TXT,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL)) == INVALID_HANDLE_VALUE) {
        handleError(__MESSAGE_PARAMS("Unable to open file for reading"));
    }

    PSID owner = getCurrentUserSid();
    LPCSTR ownerString = convertBinarySidToStringSid(owner);

    _tprintf(_T("[INFO: %s]\tWriting the current SID to '%s'\n"), PREFIX, F_P2_TXT);
    DWORD dwBytesWritten = 0;
    if (WriteFile(handle,
                  ownerString,
                  strlen((const char *) ownerString),
                  &dwBytesWritten,
                  NULL) == FALSE) {
        logSuccessOrFailure("Failed to wrote the current SID to the file.\n", hFile);
        handleError(__MESSAGE_PARAMS("Unable to write the SID to file"));
    } else {
        logSuccessOrFailure("Successfully wrote the current SID to the file.\n", hFile);
    }

    CloseHandle(handle);
}

void createRegistryKey(void) {
    HKEY hkSub = NULL;
    DWORD dwDisposition;
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       REG_KEY_PATH,
                       0,
                       _T(""),
                       0,
                       KEY_WRITE,
                       NULL,
                       &hkSub,
                       &dwDisposition) != ERROR_SUCCESS) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            _tprintf(_T("[INFO: %s]\tRegistry key 'HCKU\\%s' already exists. No action taken.\n"),
                     PREFIX, REG_KEY_PATH);
        } else {
            logSuccessOrFailure("Failed to create the registry key.\n", hFile);
            handleError(__MESSAGE_PARAMS("Cannot create registry key with given security attributes"));
        }
    } else {
        logSuccessOrFailure("Successfully created the registry key.\n", hFile);
        _tprintf(_T("[INFO: %s]\tSuccessfully created registry key 'HCKU\\%s'.\n"), PREFIX, REG_KEY_PATH);
    }

    LPCSTR currentSidString = convertBinarySidToStringSid(getCurrentUserSid());
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

    _tprintf(_T("[INFO: %s]\tCurrent user's SID: %s\n"), PREFIX, currentSidString);
    _tprintf(_T("[INFO: %s]\tWriting the last 6 digits of the SID, skipping '-'s -> %ld\n"),
             PREFIX, lastSixDigits);
    if (RegSetValueEx(hkSub,
                       REG_SUB_KEY_NAME,
                       0,
                       REG_DWORD,
                       (const byte*) &lastSixDigits,
                       sizeof lastSixDigits)) {
        logSuccessOrFailure("Failed to set the last 6 digits to the registry sub key.\n", hFile);
        handleError(__MESSAGE_PARAMS("Unable to create registry sub key"));
    } else {
        logSuccessOrFailure("Successfully set the last 6 digits to the registry sub key.\n", hFile);
    }
}

void launchCustomExecutable(void) {
    STARTUPINFO info = {sizeof(info)};
    PROCESS_INFORMATION processInfo;

    if (CreateProcess(P3_EXE_PATH,         /* Module name */
                      NULL,                /* Command line */
                      NULL,
                      NULL,
                      TRUE,
                      0,
                      NULL,
                      NULL,
                      &info,
                      &processInfo) == 0) {
        logSuccessOrFailure("Failed to execute P3.\n", hFile);
        handleError(__MESSAGE_PARAMS("Unable to launch P3"));
    } else {
        logSuccessOrFailure("Successfully executed P3.\n", hFile);
    };

    /* Make sure to wait for it to finish */
    WaitForSingleObject(processInfo.hProcess, INFINITE);
}
