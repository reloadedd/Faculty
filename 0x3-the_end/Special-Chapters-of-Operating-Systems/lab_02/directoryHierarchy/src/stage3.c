/*  Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>. Some rights reversed. */
#include "../lib/stage3.h"


void create_registry_key(LPCSTR path) {
    HKEY registry_handle = NULL;

    if (RegCreateKeyExA(HKEY_CURRENT_USER,
                        "SOFTWARE\\CSSO\\Tema1",
                        0,
                        NULL,
                        REG_OPTION_NON_VOLATILE,
                        KEY_SET_VALUE | KEY_CREATE_SUB_KEY,
                        NULL,
                        &registry_handle,
                        NULL) != ERROR_SUCCESS) {
        fprintf(stderr, "ERROR\tUnable to create registry key. Error code: %lu\n", GetLastError());
        return;
    }

    if (RegSetValueExA(registry_handle,
                       "PathToSummaryFile",
                       0,
                       REG_SZ,
                       (PBYTE) path,
                       strlen(path))) {
        fprintf(stderr, "ERROR\tUnable to create registry sub key. Error code: %lu\n", GetLastError());
        return;
    }

    HANDLE handle;
    if ((handle = CreateFile(path,
                             GENERIC_READ,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR\tUnable to create handle for opening file '%s'. Code: %lu\n",
                path, GetLastError());
        return;
    }

    DWORD size;
    if ((size = GetFileSize(handle, NULL)) == INVALID_FILE_SIZE) {
        fprintf(stderr, "ERROR\tUnable to get file size of the file '%s'.\n", path);
        return;
    }

    if (RegSetValueExA(registry_handle,
                       "SummaryFileSize",
                       0,
                       REG_DWORD,
                       (LPBYTE) &size,
                       sizeof size)) {
        fprintf(stderr, "ERROR\tUnable to create registry sub key. Error code: %lu\n", GetLastError());
        return;
    }
}


void trigger_stage_three_protocol(LPCSTR bridge) {
    printf("INFO\t==== Stage Three initiated. ====\n");
    printf("INFO\tSummary file path: %s\n", bridge);

    create_registry_key(bridge);
}