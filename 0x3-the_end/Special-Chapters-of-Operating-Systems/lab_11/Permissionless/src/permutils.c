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
#include "../include/permutils.h"


/*--=== FUNCTION DEFINITIONS ===--*/
PSID getCurrentUserSid() {
    /*
     * Description
     * -----------
     *  Get the SID for the user running this process.
     *
     * References
     * ----------
     *  Inspired & adapted from:
     *  https://social.msdn.microsoft.com/forums/vstudio/en-US/6b23fff0-773b-4065-bc3f-d88ce6c81eb0/get-user-sid-in-unmanaged-c
     */

    /* Open the access token associated with the calling process */
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        handleError(__MESSAGE_PARAMS("Unable to open process token"));
    }

    /* Get the size of the memory buffer needed for the SID */
    DWORD dwBufferSize = 0;
    if (!GetTokenInformation(hToken,
                             TokenUser,
                             NULL,
                             0,
                             &dwBufferSize) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER)) {
        CloseHandle(hToken);
        handleError(__MESSAGE_PARAMS("Unable to get token information"));
    }

    /* Allocate buffer for user token data */
    LPSTR buffer = malloc(dwBufferSize);
    PTOKEN_USER pTokenUser = (PTOKEN_USER) &buffer[0];

    /* Retrieve the token information in a TOKEN_USER structure */
    if (!GetTokenInformation(
            hToken,
            TokenUser,
            pTokenUser,
            dwBufferSize,
            &dwBufferSize)) {
        CloseHandle(hToken);
        handleError(__MESSAGE_PARAMS("Unable to get token information"));
    }

    /* Check if SID is valid */
    if (!IsValidSid(pTokenUser->User.Sid)) {
        CloseHandle(hToken);
        handleError(__MESSAGE_PARAMS("Invalid SID received"));
    }

    CloseHandle(hToken);
    return pTokenUser->User.Sid;
}

LPCSTR convertBinarySidToStringSid(PSID sid) {
    /*
     * Description
     * -----------
     *  Convert a binary SID to its string representation.
     */
    LPSTR buffer = malloc(SECURITY_MAX_SID_SIZE);
    ConvertSidToStringSid(sid, &buffer);

    return (LPCSTR) buffer;
}
