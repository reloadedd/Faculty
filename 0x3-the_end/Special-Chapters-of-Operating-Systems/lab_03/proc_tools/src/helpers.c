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

#include "../lib/helpers.h"

void display_help_menu(void) {
    printf("==================================================\n");
    printf("\t[*] Process Tinker and Explorer\n");
    printf("\t\t\tby reloadedd\n");
    printf("==================================================\n");
    printf("\nAvailable options:\n");
    printf("\t--help\t\t\t\t\tPrints this menu\n");
    printf("\t--list-processes\t\t\tList all processes currently running\n");
    printf("\t--kill-process PID\t\t\tKill the process having the given PID <pid>\n");
    printf("\t--list-modules PID\t\t\tList all modules associated with the given PID\n");
    printf("\t--pause-process PID\t\t\tSuspend all threads of the given PID\n");
    printf("\t--launch-executable PATH\t\tLaunch the executable found at the given path\n");
    printf("\t--memory-map INPUT_FILE OUTPUT_FILE\tCreate a memory mapping and handle the two files\n");
}

void handle_error(const char* error) {
    fprintf(stderr, "ERROR\t%s [code=%ld]\n", error, GetLastError());
    exit(EXIT_FAILURE);
}

DWORD parse_process_id(const char* processID) {
    if ((strlen(processID) == 0 && strcmp(processID, MAX_PROCESS_ID_VALUE) > 0) || strlen(processID) > 10) {
        printf("ERROR\tThe PID [%s] exceeds the maximum allowed PID value (%s).\n",
               processID, MAX_PROCESS_ID_VALUE);
        exit(EXIT_FAILURE);
    }

    char* unused_ptr;
    return (DWORD) strtoul(processID, &unused_ptr, 10);
}

/* Taken from: https://docs.microsoft.com/en-us/windows/win32/secauthz/enabling-and-disabling-privileges-in-c--
 *
 * Sets a privilege to an Access Token Handle.
 *  Note: The parent process must have permission to set that higher privilege.
 */
BOOL SetPrivilege(
        HANDLE hToken,          // access token handle
        LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
        BOOL bEnablePrivilege   // to enable or disable privilege
)
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if ( !LookupPrivilegeValue(
            NULL,            // lookup privilege on local system
            lpszPrivilege,   // privilege to lookup
            &luid ) )        // receives LUID of privilege
    {
        printf("LookupPrivilegeValue error: %lu\n", GetLastError() );
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.
    if ( !AdjustTokenPrivileges(
            hToken,
            FALSE,
            &tp,
            sizeof(TOKEN_PRIVILEGES),
            (PTOKEN_PRIVILEGES) NULL,
            (PDWORD) NULL) )
    {
        printf("AdjustTokenPrivileges error: %lu\n", GetLastError() );
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        printf("The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}