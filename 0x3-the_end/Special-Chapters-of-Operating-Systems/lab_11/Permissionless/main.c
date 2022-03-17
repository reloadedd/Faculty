#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <aclapi.h>
#include "include/funcutils.h"
#include "include/constants.h"

void launchCustomExecutable(void);


int _tmain(int argc, TCHAR *argv[]) {
    _tprintf(_T("[INFO: %s]\tLaunching '%s'...\n"), MAIN_PREFIX, P1_EXE_PATH);
    launchExecutable(P1_EXE_PATH);

    _tprintf(_T("[INFO: %s]\tLaunching '%s'...\n"), MAIN_PREFIX, P2_EXE_PATH);
    launchExecutable(P2_EXE_PATH);

    _tprintf(_T("[INFO: %s]\tLaunching '%s' again...\n"), MAIN_PREFIX, P2_EXE_PATH);
    launchCustomExecutable();

    return 0;
}

void launchCustomExecutable(void) {
    PSID owner = getCurrentUserSid();
    EXPLICIT_ACCESS ea;

    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions  = PROCESS_CREATE_PROCESS | PROCESS_CREATE_THREAD;
    ea.grfAccessMode         = DENY_ACCESS;
    ea.grfInheritance        = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea.Trustee.TrusteeForm   = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType   = TRUSTEE_IS_USER;
    ea.Trustee.ptstrName     = owner;

    PACL pACL = NULL;
    if (SetEntriesInAcl(1, &ea, NULL, &pACL) != ERROR_SUCCESS) {
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

    STARTUPINFO info = {sizeof(info)};
    PROCESS_INFORMATION processInfo;

    if (CreateProcess(P2_EXE_PATH,         /* Module name */
                      NULL,                /* Command line */
                      &sa,
                      &sa,
                      TRUE,
                      0,
                      NULL,
                      NULL,
                      &info,
                      &processInfo) == 0) {
        handleError(__MESSAGE_PARAMS("Unable to launch P1"));
    }

    /* Make sure to wait for it to finish */
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    LocalFree(pSecurityDescriptor);
}
