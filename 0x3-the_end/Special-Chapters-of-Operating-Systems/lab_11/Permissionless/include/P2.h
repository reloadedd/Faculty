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

#ifndef PERMISSIONLESS_P2_H
#define PERMISSIONLESS_P2_H

/*--=== INCLUDES ===--*/
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "funcutils.h"
#include "permutils.h"
#include "constants.h"


/*--=== MACROS ===--*/
#define PREFIX              _TEXT("P2")
#define F_P2_TXT            _TEXT("RESTRICTED WRITE\\P2.txt")
#define LOGS                _TEXT("LOGS\\P2.txt")
#define REG_KEY_PATH        _TEXT("Software\\CSSO\\Tema2")
#define REG_SUB_KEY_NAME    _TEXT("P2")


/*--=== FUNCTION DECLARATIONS ===--*/
extern void writeCurrentSidToFile(void);
extern void createRegistryKey(void);
extern void launchCustomExecutable(void);

#endif //PERMISSIONLESS_P2_H
