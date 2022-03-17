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
#include "../lib/utils.h"


/*--=== FUNCTION DEFINITIONS ===--*/
void handleError(const char* error) {
    _ftprintf(stderr, "ERROR\t%s [code=%ld]\n", error, GetLastError());
    exit(EXIT_FAILURE);
}

void createAndSetWorkingDirectory(LPCSTR directory) {
    if (CreateDirectory(directory, NULL) == 0) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            handleError("Unable to create directory (@ utils.c)");
        }
    }

    if (SetCurrentDirectory(directory) == 0) {
        handleError("Unable to set current directory (@ utils.c)");
    }
}


void getRequestAndSaveResponse(LPCSTR remoteUrl, LPCSTR remotePath, LPCSTR localPath) {
    HINTERNET hInternetOpen;
    HINTERNET hInternetConnect;
    HINTERNET hHttpRequest;

    if ((hInternetOpen = InternetOpen(USER_AGENT,
                                      INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY,
                                      NULL,
                                      NULL,
                                      (DWORD) 0)) == NULL) {
        handleError("Unable to initialize connection (@ utils.c)");
    }

    if ((hInternetConnect = InternetConnect(hInternetOpen,
                                            remoteUrl,
                                            (INTERNET_PORT) REMOTE_PORT,
                                            NULL,
                                            NULL,
                                            INTERNET_SERVICE_HTTP,
                                            (DWORD) 0,
                                            (DWORD_PTR) 0)) == NULL) {
        handleError("Unable to connect to the remote URL (@ utils.c)");
    }

    if ((hHttpRequest = HttpOpenRequest(hInternetConnect,
                                        TEXT("GET"),
                                        remotePath,
                                        TEXT("HTTP/1.1"),
                                        TEXT("from.nowhere"),
                                        (LPCSTR []) {TEXT("text/*"), NULL},
                                        INTERNET_FLAG_RELOAD,
                                        (DWORD_PTR) 0)) == NULL) {
        handleError("Unable to create HTTP request (@ getRequestAndSaveResponse()/utils.c)");
    }

    if (HttpSendRequest(hHttpRequest,
                        NULL,
                        (DWORD) 0,
                        NULL,
                        (DWORD) 0) == FALSE) {
        handleError("Unable to send HTTP request (@ getRequestAndSaveResponse()/utils.c)");
    }

    HANDLE handle;
    DWORD bytesWritten;
    DWORD bytesRead;
    TCHAR *chunk = malloc((sizeof(TCHAR) * CHUNK_SIZE) + 1);

    /* Open file for writing -- Create or overwrite content */
    if ((handle = CreateFile(localPath,
                             GENERIC_WRITE,
                             0,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL)) == INVALID_HANDLE_VALUE) {
        handleError("Unable to create file (@ utils.c)");
    }

    while (!(InternetReadFile(hHttpRequest,
                              chunk,
                              CHUNK_SIZE,
                              &bytesRead) == TRUE && bytesRead == 0)) {
        if (WriteFile(handle,
                      chunk,
                      bytesRead,
                      &bytesWritten,
                      NULL) == FALSE) {
            handleError("Unable to write to file (@ utils.c)");
        }
    }

    free(chunk);
    CloseHandle(handle);
    InternetCloseHandle(hInternetConnect);
    InternetCloseHandle(hInternetOpen);
    InternetCloseHandle(hHttpRequest);
}

void createSubFolders(const TCHAR* filename) {
    HANDLE hFile;
    if ((hFile = CreateFile(filename,
                            GENERIC_READ,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL)) == INVALID_HANDLE_VALUE) {
        handleError("Unable to open file for reading (@ utils.c)");
    }

    DWORD size;
    DWORD bytesRead = 0;
    if ((size = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE) {
        handleError("Unable to get file size (@ utils.c)");
    }
    TCHAR* buffer = malloc((size + 1) * sizeof(TCHAR));
    ZeroMemory(buffer, size + 1);

    if (ReadFile(hFile,
                 buffer,
                 size,
                 &bytesRead,
                 NULL) == FALSE) {
        handleError("Unable to read from file (@ utils.c)");
    }

    /* Create a new directory where the sub-folders will be created and set that directory as CWD. */
    createAndSetWorkingDirectory(CONFIG_DIR);

    /* Get the lines */
    TCHAR delimiters[] = "\n";
    TCHAR* context = NULL;
    TCHAR* token = strtok_s(buffer, delimiters, &context);

    while (token != NULL) {
        if (CreateDirectory(token, NULL) == 0) {
            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                handleError("Unable to create sub-folder (@ utils.c)");
            }
        }

        token = strtok_s(NULL, delimiters, &context);
    }

    CloseHandle(hFile);
    free(buffer);
}

/* Credits: https://stackoverflow.com/a/5804935 */
LPCSTR gnuBasename(LPCSTR path) {
    LPSTR base = strrchr(path, '/');
    return base ? base+1 : path;
}

BOOL downloadHttps(LPCSTR remoteUrl, LPCSTR remotePath, LPCSTR localPath, LPDWORD size) {
    HINTERNET hInternetOpen;
    HINTERNET hInternetConnect;
    HINTERNET hHttpRequest;

    if ((hInternetOpen = InternetOpen(USER_AGENT,
                                      INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY,
                                      NULL,
                                      NULL,
                                      (DWORD) 0)) == NULL) {
        return FALSE;
    }

    if ((hInternetConnect = InternetConnect(hInternetOpen,
                                            remoteUrl,
                                            (INTERNET_PORT) 443,
                                            NULL,
                                            NULL,
                                            INTERNET_SERVICE_HTTP,
                                            (DWORD) 0,
                                            (DWORD_PTR) 0)) == NULL) {
        return FALSE;
    }

    if ((hHttpRequest = HttpOpenRequest(hInternetConnect,
                                        TEXT("GET"),
                                        remotePath,
                                        TEXT("HTTP/1.1"),
                                        TEXT("from.nowhere"),
                                        (LPCSTR []) {TEXT("text/*"), NULL},
                                        INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE,
                                        (DWORD_PTR) 0)) == NULL) {
        return FALSE;
    }

    if (HttpSendRequest(hHttpRequest,
                        NULL,
                        (DWORD) 0,
                        NULL,
                        (DWORD) 0) == FALSE) {
        return FALSE;
    }

    HANDLE handle;
    DWORD bytesWritten;
    DWORD bytesRead;
    TCHAR *chunk = malloc((sizeof(TCHAR) * CHUNK_SIZE) + 1);

    /* Open file for writing -- Create or overwrite content */
    if ((handle = CreateFile(localPath,
                             GENERIC_WRITE,
                             0,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL)) == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    *size = 0;
    while (!(InternetReadFile(hHttpRequest,
                              chunk,
                              CHUNK_SIZE,
                              &bytesRead) == TRUE && bytesRead == 0)) {
        if (WriteFile(handle,
                      chunk,
                      bytesRead,
                      &bytesWritten,
                      NULL) == FALSE) {
            return FALSE;
        }
        *size += bytesWritten;
    }

    free(chunk);
    CloseHandle(handle);
    InternetCloseHandle(hInternetConnect);
    InternetCloseHandle(hInternetOpen);
    InternetCloseHandle(hHttpRequest);

    return TRUE;
}

void postRequest(LPCSTR remoteUrl, LPCSTR remotePath, LPCSTR data, DWORD size) {
    HINTERNET hInternetOpen;
    HINTERNET hInternetConnect;
    HINTERNET hHttpRequest;

    if ((hInternetOpen = InternetOpen(USER_AGENT,
                                      INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY,
                                      NULL,
                                      NULL,
                                      (DWORD) 0)) == NULL) {
        handleError("Unable to initialize connection (@ utils.c)");
    }

    if ((hInternetConnect = InternetConnect(hInternetOpen,
                                            remoteUrl,
                                            (INTERNET_PORT) REMOTE_PORT,
                                            NULL,
                                            NULL,
                                            INTERNET_SERVICE_HTTP,
                                            (DWORD) 0,
                                            (DWORD_PTR) 0)) == NULL) {
        handleError("Unable to connect to the remote URL (@ postRequest()/utils.c)");
    }

    if ((hHttpRequest = HttpOpenRequest(hInternetConnect,
                                        TEXT("POST"),
                                        remotePath,
                                        TEXT("HTTP/1.1"),
                                        TEXT("from.nowhere"),
                                        (LPCSTR []) {TEXT("text/*"), NULL},
                                        INTERNET_FLAG_RELOAD,
                                        (DWORD_PTR) 0)) == NULL) {
        handleError("Unable to create HTTP request (@ postRequest()/utils.c)");
    }

    TCHAR headers[] = "Content-Type: application/x-www-form-urlencoded";
    if (HttpSendRequest(hHttpRequest,
                        headers,
                        -1, /* Compute the size automatically */
                        (LPVOID) data,
                        size) == FALSE) {
        handleError("Unable to send POST HTTP request (@ utils.c)");
    }

    InternetCloseHandle(hInternetConnect);
    InternetCloseHandle(hInternetOpen);
    InternetCloseHandle(hHttpRequest);
}

void downloadAndSaveFiles(const TCHAR* filename) {
    HANDLE hFile;
    if ((hFile = CreateFile(filename,
                            GENERIC_READ,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL)) == INVALID_HANDLE_VALUE) {
        handleError("Unable to open file for reading (@ utils.c)");
    }

    DWORD size;
    DWORD bytesRead = 0;
    if ((size = GetFileSize(hFile, NULL)) == INVALID_FILE_SIZE) {
        handleError("Unable to get file size (@ utils.c)");
    }
    TCHAR* buffer = malloc((size + 1) * sizeof(TCHAR));
    ZeroMemory(buffer, size + 1);

    if (ReadFile(hFile,
                 buffer,
                 size,
                 &bytesRead,
                 NULL) == FALSE) {
        handleError("Unable to read from file (@ utils.c)");
    }

    TCHAR delimiters[] = "\n";
    TCHAR* context = NULL;
    TCHAR* token = strtok_s(buffer, delimiters, &context);
    DWORD successes = 0;
    DWORD failures = 0;
    DWORD totalSize = 0;

    while (token != NULL) {
        TCHAR delimiters2[] = "||";
        TCHAR* context2 = NULL;
        TCHAR* token2 = strtok_s(token, delimiters2, &context2);

        TCHAR* directory = token2;
        token2 = strtok_s(NULL, delimiters2, &context2);
        TCHAR* link = token2;

        LPCSTR _filename = gnuBasename(link);
        TCHAR fullDomainName[255];
        LPSTR withoutProtocolPrefix = strchr(link, '/');
        int i = 2;
        for (; withoutProtocolPrefix[i]; ++i) {
            if (withoutProtocolPrefix[i] == '/') {
                break;
            }
            fullDomainName[i - 2] = withoutProtocolPrefix[i];
        }
        fullDomainName[i - 2] = '\0';
        LPSTR remotePath = strchr(withoutProtocolPrefix + 2, '/');
        LPSTR localPath = malloc(strlen(directory) + strlen(_filename) + 2);
        snprintf(localPath, strlen(directory) + strlen(_filename) + 2, "%s\\%s", directory, _filename);

        printf("INFO\tNow downloading [%s]...", link);
        DWORD _size;
        if (downloadHttps(fullDomainName, remotePath, localPath, &_size) == TRUE) {
            successes++;
        } else {
            failures++;
        }
        printf("%ld bytes\n", _size);
        totalSize += _size;

        token = strtok_s(NULL, delimiters, &context);
    }

    printf("INFO\tSuccesses: %ld | Failures: %ld | Total size: %ld bytes\n", successes, failures, totalSize);
    /* len('id=310910401ESL191074&size=1234567890&s=1234567890&e=1234567890') -> 63 */
    TCHAR postData[65];
    snprintf(postData, 65, "id=%s&size=%ld&s=%ld&e=%ld", NR_MATRICOL, totalSize, successes, failures);
    postRequest(REMOTE_URL, POST_REMOTE_PATH, postData, 65);

    CloseHandle(hFile);
    free(buffer);
}
