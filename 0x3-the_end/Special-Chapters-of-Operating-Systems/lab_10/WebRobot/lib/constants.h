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

#ifndef WEBROBOT_CONSTANTS_H
#define WEBROBOT_CONSTANTS_H


#define REMOTE_URL          TEXT("cssohw.herokuapp.com")
//#define REMOTE_URL          TEXT("127.0.0.1")
#define POST_REMOTE_PATH    TEXT("/submit")
//#define REMOTE_PORT         (DWORD) 8000
#define REMOTE_PORT         (DWORD) 80
#define NR_MATRICOL         TEXT("310910401ESL191074")
#define CONFIG_FILE         TEXT("config.txt")
#define CONFIG_DIR          TEXT("config")
#define EXECUTABLES_FILE    TEXT("executabile.txt")
#define USER_AGENT          TEXT("310910401ESL191074's Money Robot")
#define ROOT_DIR            TEXT("results")
#define CHUNK_SIZE          (DWORD) 4096


#endif //WEBROBOT_CONSTANTS_H
