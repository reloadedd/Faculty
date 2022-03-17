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
#include "../include/P1.h"


int _tmain(int argc, TCHAR *argv[]) {
    /* Create the 'RESTRICTED WRITE' directory */
    createRestrictedWriteDir();

    /* Create the 'LOGS' directory */
    createLogsDir();

    /* Create the 'HKEY_CURRENT_USER\Software\CSSO\Tema5' registry key */
    createRegistryKey();
}

void createRestrictedWriteDir(void) {
    PSID owner = getCurrentUserSid();
    PSID everyone = malloc(SECURITY_MAX_SID_SIZE);
    DWORD sidSize = SECURITY_MAX_SID_SIZE;

    if (CreateWellKnownSid(WinWorldSid, NULL, everyone, &sidSize) == FALSE) {
        handleError(__MESSAGE_PARAMS("Unable to create well known SID for everyone"));
    }

    EXPLICIT_ACCESS ea[3];
    ZeroMemory(&ea, 3 * sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions  = FILE_GENERIC_READ;
    ea[0].grfAccessMode         = SET_ACCESS;
    ea[0].grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea[0].Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType   = TRUSTEE_IS_USER;
    ea[0].Trustee.ptstrName     = owner;

    ea[1].grfAccessPermissions  = FILE_GENERIC_WRITE;
    ea[1].grfAccessMode         = SET_ACCESS;
    ea[1].grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea[1].Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType   = TRUSTEE_IS_USER;
    ea[1].Trustee.ptstrName     = owner;

    ea[2].grfAccessPermissions  = FILE_GENERIC_READ;
    ea[2].grfAccessMode         = SET_ACCESS;
    ea[2].grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea[2].Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea[2].Trustee.TrusteeType   = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[2].Trustee.ptstrName     = everyone;

    PACL pACL = NULL;
    if (SetEntriesInAcl(3, ea, NULL, &pACL) != ERROR_SUCCESS) {
        handleError(__MESSAGE_PARAMS("Unable to set entries in ACL"));
    }

    PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
    if ((pSecurityDescriptor = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH)) == NULL) {
        handleError(__MESSAGE_PARAMS("Unable to allocate security descriptor"));
    }

    if (!InitializeSecurityDescriptor(pSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION)) {
        handleError(__MESSAGE_PARAMS("Unable to initialize security descriptor"));
    }

    /* Add the ACL to the security descriptor. */
    if (!SetSecurityDescriptorDacl(pSecurityDescriptor, TRUE, pACL, FALSE)) {
        handleError(__MESSAGE_PARAMS("Unable to set security descriptor DACLs"));
    }

    /* Initialize a security attributes structure. */
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof (SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = pSecurityDescriptor;
    sa.bInheritHandle = FALSE;

    if (CreateDirectory(DIR_RESTRICTED_WRITE, &sa) == FALSE) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            _tprintf(_T("[INFO: %s]\tFolder '%s' already exists. No action taken.\n"), PREFIX, DIR_RESTRICTED_WRITE);
        } else {
            handleError(__MESSAGE_PARAMS("Cannot create directory with given security attributes"));
        }
    } else {
        _tprintf(_T("[INFO: %s]\tSuccessfully created folder '%s'.\n"), PREFIX, DIR_RESTRICTED_WRITE);
    }

    LocalFree(pSecurityDescriptor);
}

void createLogsDir(void) {
    PSID everyone = malloc(SECURITY_MAX_SID_SIZE);
    DWORD sidSize = SECURITY_MAX_SID_SIZE;

    if (CreateWellKnownSid(WinWorldSid, NULL, everyone, &sidSize) == FALSE) {
        handleError(__MESSAGE_PARAMS("Unable to create well known SID for everyone"));
    }

    EXPLICIT_ACCESS ea[2];
    ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions  = FILE_GENERIC_READ;
    ea[0].grfAccessMode         = SET_ACCESS;
    ea[0].grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea[0].Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType   = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName     = everyone;

    ea[1].grfAccessPermissions  = FILE_GENERIC_WRITE;
    ea[1].grfAccessMode         = SET_ACCESS;
    ea[1].grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea[1].Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType   = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[1].Trustee.ptstrName     = everyone;

    PACL pACL = NULL;
    if (SetEntriesInAcl(2, ea, NULL, &pACL) != ERROR_SUCCESS) {
        handleError(__MESSAGE_PARAMS("Unable to set entries in ACL"));
    }

    PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
    if ((pSecurityDescriptor = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH)) == NULL) {
        handleError(__MESSAGE_PARAMS("Unable to allocate security descriptor"));
    }

    if (!InitializeSecurityDescriptor(pSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION)) {
        handleError(__MESSAGE_PARAMS("Unable to initialize security descriptor"));
    }

    /* Add the ACL to the security descriptor. */
    if (!SetSecurityDescriptorDacl(pSecurityDescriptor, TRUE, pACL, FALSE)) {
        handleError(__MESSAGE_PARAMS("Unable to set security descriptor DACLs"));
    }

    /* Initialize a security attributes structure. */
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof (SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = pSecurityDescriptor;
    sa.bInheritHandle = FALSE;

    if (CreateDirectory(DIR_LOGS, &sa) == FALSE) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            _tprintf(_T("[INFO: %s]\tFolder '%s' already exists. No action taken.\n"), PREFIX, DIR_LOGS);
        } else {
            handleError(__MESSAGE_PARAMS("Cannot create directory with given security attributes"));
        }
    } else {
        _tprintf(_T("[INFO: %s]\tSuccessfully created folder '%s'.\n"), PREFIX, DIR_LOGS);
    }

    LocalFree(pSecurityDescriptor);
}

void createRegistryKey() {
    PSID owner = getCurrentUserSid();

    EXPLICIT_ACCESS ea[2];
    ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions  = KEY_READ;
    ea[0].grfAccessMode         = SET_ACCESS;
    ea[0].grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea[0].Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType   = TRUSTEE_IS_USER;
    ea[0].Trustee.ptstrName     = owner;

    ea[1].grfAccessPermissions  = KEY_WRITE;
    ea[1].grfAccessMode         = SET_ACCESS;
    ea[1].grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea[1].Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType   = TRUSTEE_IS_USER;
    ea[1].Trustee.ptstrName     = owner;

    PACL pACL = NULL;
    if (SetEntriesInAcl(2, ea, NULL, &pACL) != ERROR_SUCCESS) {
        handleError(__MESSAGE_PARAMS("Unable to set entries in ACL"));
    }

    PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
    if ((pSecurityDescriptor = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH)) == NULL) {
        handleError(__MESSAGE_PARAMS("Unable to allocate security descriptor"));
    }

    if (!InitializeSecurityDescriptor(pSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION)) {
        handleError(__MESSAGE_PARAMS("Unable to initialize security descriptor"));
    }

    /* Add the ACL to the security descriptor. */
    if (!SetSecurityDescriptorDacl(pSecurityDescriptor, TRUE, pACL, FALSE)) {
        handleError(__MESSAGE_PARAMS("Unable to set security descriptor DACLs"));
    }

    /* Initialize a security attributes structure. */
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof (SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = pSecurityDescriptor;
    sa.bInheritHandle = FALSE;

    HKEY hkSub = NULL;
    DWORD dwDisposition;
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       REG_KEY_PATH,
                       0,
                       _T(""),
                       0,
                       KEY_READ | KEY_WRITE,
                       &sa,
                       &hkSub,
                       &dwDisposition) != ERROR_SUCCESS) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            _tprintf(_T("[INFO: %s]\tRegistry key 'HCKU\\%s' already exists. No action taken.\n"),
                     PREFIX, REG_KEY_PATH);
        } else {
            handleError(__MESSAGE_PARAMS("Cannot create registry key with given security attributes"));
        }
    } else {
        _tprintf(_T("[INFO: %s]\tSuccessfully created registry key 'HCKU\\%s'.\n"), PREFIX, REG_KEY_PATH);
    }

    LocalFree(pSecurityDescriptor);
}