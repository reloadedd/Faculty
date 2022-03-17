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

#ifndef PERMISSIONLESS_FUNCUTILS_H
#define PERMISSIONLESS_FUNCUTILS_H

/*--=== INCLUDES ===--*/
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "permutils.h"

/*--=== MACROS ===--*/
#define __MESSAGE_PARAMS(string)   _TEXT(string), __LINE__, __FILE__

/*--=== FUNCTION DECLARATIONS ===--*/
extern void handleError(LPCSTR error, DWORD lineNumber, LPCSTR fullFileName);
extern LPCSTR basename(LPCSTR path);
extern void launchExecutable(LPCSTR path);
extern void logSuccessOrFailure(LPCSTR message, HANDLE hFile);

#endif //PERMISSIONLESS_FUNCUTILS_H