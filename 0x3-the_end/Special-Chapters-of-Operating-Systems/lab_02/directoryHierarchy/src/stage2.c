/*  Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>. Some rights reversed. */
#include "../lib/stage2.h"

#define NO_OF_FILES         5
#define MAX_DWORD_FILE_SIZE 11


LPCSTR FILES[] = {"HKLM.txt", "HKCC.txt", "HKCR.txt", "HCKU.txt", "HKU.txt"};
LPCSTR SUMMARY_FILE = "summary.txt";


LPSTR get_cwd(void) {
    LPSTR current_directory = malloc((MAX_PATH + 1) * sizeof(CHAR));
    GetCurrentDirectory(MAX_PATH, current_directory);

    return current_directory;
}

short int check_path(LPCSTR path) {
    /* Checks if the path given as argument exists.
     *
     * Returns:
     *  -1, if the path does not exist
     *   1, if the path exists and is a directory
     *   2, if the path exists and is a file
     */
    struct stat info;

    if (stat(path, &info) != 0) {
        return PATH_DOES_NOT_EXIST;
    } else if (info.st_mode & S_IFDIR) {
        return PATH_IS_DIRECTORY;
    } else if (info.st_mode & S_IFREG) {
        return PATH_IS_REG_FILE;
    }

    /* Does nothing but gets rid of the warning */
    return PATH_DOES_NOT_EXIST;
}

void enumerate_registry_metadata(HKEY hive, LPCSTR lpFileName) {
    HKEY hRegKey;

    if (RegOpenKeyEx(hive,
                     NULL,
                     0,
                     KEY_READ,
                     &hRegKey) != ERROR_SUCCESS) {
        fprintf(stderr, "ERROR\tUnable to open registry key hive. Code: %lu\n", GetLastError());
        return;
    }

    /* Taken from the official M$ documentation and changed names a bit for ease of use. */
    TCHAR    achClass[MAX_PATH] = TEXT("");   // buffer for class name
    DWORD    cchClassName = MAX_PATH;               // size of class string
    DWORD    lpcSubKeys = 0;                        // number of subkeys
    DWORD    lpcbMaxSubKeyLen;                      // longest subkey size
    DWORD    lpcbMaxClassLen;                       // longest class string
    DWORD    lpcValues;                             // number of values for key
    DWORD    lpcbMaxValueNameLen;                   // longest value name
    DWORD    lpcbMaxValueLen;                       // longest value data
    DWORD    lpcbSecurityDescriptor;                // size of security descriptor
    FILETIME lpftLastWriteTime;                     // last write time

    if (RegQueryInfoKey(
            hRegKey,                 // key handle
            achClass,                // buffer for class name
            &cchClassName,           // size of class string
            NULL,                    // reserved
            &lpcSubKeys,             // number of subkeys
            &lpcbMaxSubKeyLen,       // longest subkey size
            &lpcbMaxClassLen,        // longest class string
            &lpcValues,              // number of values for this key
            &lpcbMaxValueNameLen,    // longest value name
            &lpcbMaxValueLen,        // longest value data
            &lpcbSecurityDescriptor, // security descriptor
            &lpftLastWriteTime)      // last write time
            != ERROR_SUCCESS) {
        fprintf(stderr, "ERROR\tUnable to read metadata from registry hive. Code: %lu\n", GetLastError());
        return;
    }

    HANDLE handle;
    DWORD dwords[] = {lpcSubKeys,
                      lpcbMaxSubKeyLen,
                      lpcbMaxClassLen,
                      lpcValues,
                      lpcbMaxValueNameLen,
                      lpcbMaxValueLen,
                      lpcbSecurityDescriptor};

    /* Open file for writing -- Create or overwrite content */
    if ((handle = CreateFile(lpFileName,
                             GENERIC_WRITE,
                             0,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR\tUnable to create handle for opening file '%s'. Code: %lu\n",
                lpFileName, GetLastError());
        return;
    }

    for (int i = 0; i < 7; ++i) {
        DWORD bytes_written;
        CHAR buffer[MAX_DWORD_FILE_SIZE];
        _ultoa_s(dwords[i], buffer, MAX_DWORD_FILE_SIZE, 10);
        size_t len = strlen(buffer);
        buffer[len] = '\n';
        buffer[len + 1] = (CHAR) 0;

        if (WriteFile(handle,
                      buffer,
                      strlen(buffer),
                      &bytes_written,
                      NULL) == FALSE) {
            fprintf(stderr, "ERROR\tUnable to write to file '%s'. Code: %lu\n",
                    lpFileName, GetLastError());
            return;
        }
    }

    /* Now write the time */
    SYSTEMTIME _time = { 0 };
    if (FileTimeToSystemTime(&lpftLastWriteTime, &_time) != TRUE) {
        fprintf(stderr, "ERROR\tUnable to convert FILETIME -> SYSTEMTIME. Code: %lu\n", GetLastError());
        return;
    }

    CHAR time_result[15];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT,
                        0,
                        &_time,
                        NULL,
                        time_result,
                        100) == 0) {
        fprintf(stderr, "ERROR\tUnable to convert SYSTEMTIME to a readable string. Code: %lu\n", GetLastError());
        return;
    }

    DWORD bytes_written;
    if (WriteFile(handle,
              time_result,
              strlen(time_result),
              &bytes_written,
              NULL) == FALSE) {
        fprintf(stderr, "ERROR\tUnable to write to file '%s'. Code: %lu\n",
                lpFileName, GetLastError());
        return;
    }
    printf("INFO\tSuccessfully wrote to '%s'.\n", lpFileName);

    CloseHandle(handle);
    RegCloseKey(hRegKey);
}

void write_summary(LPCSTR base_path) {
    HANDLE handle;
    DWORD bytes_written;

    /* Open file for writing -- Create or overwrite content */
    if ((handle = CreateFile(SUMMARY_FILE,
                             GENERIC_WRITE,
                             0,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR\tUnable to create handle for opening file '%s'. Code: %lu\n",
                SUMMARY_FILE, GetLastError());
        return;
    }

    for (int i = 0; i < NO_OF_FILES; ++i) {
        HANDLE hive_file_handle;
        if ((hive_file_handle = CreateFile(FILES[i],
                                 GENERIC_READ,
                                 0,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL)) == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "ERROR\tUnable to create handle for opening file '%s'. Code: %lu\n",
                    FILES[i], GetLastError());
            return;
        }

        CHAR buffer[MAX_PATH + 1];
        memset(buffer, 0, MAX_PATH + 1);

        /* Write the base path */
        if (append_to_path(buffer, base_path) == -1) {
            fprintf(stderr, "ERROR\tUnable to append string.\n");
            return;
        }

        /* Append the file's relative path to the base path */
        if (append_to_path(buffer, FILES[i]) == -1) {
            fprintf(stderr, "ERROR\tUnable to append string.\n");
            return;
        }

        DWORD size;
        if ((size = GetFileSize(hive_file_handle, NULL)) == INVALID_FILE_SIZE) {
            fprintf(stderr, "ERROR\tUnable to get file size of the file '%s'.\n", FILES[i]);
            return;
        }

        /* Supports file sizes up to 10 digits */
        CHAR file_size[MAX_DWORD_FILE_SIZE];
        _ultoa_s(size, file_size, MAX_DWORD_FILE_SIZE, 10);

        /* Add a tab */
        DWORD len = strlen(buffer);
        buffer[len] = '\t';
        buffer[len + 1] = (CHAR) 0;

        if (append_to_path(buffer, file_size) == -1) {
            fprintf(stderr, "ERROR\tUnable to append string.\n");
            return;
        }

        /* Add a nice and shiny newline */
        len = strlen(buffer);
        buffer[len] = '\n';
        buffer[len + 1] = (CHAR) 0;

        if (WriteFile(handle,
                      buffer,
                      strlen((const char *) buffer),
                      &bytes_written,
                      NULL) == FALSE) {
            fprintf(stderr, "ERROR\tUnable to write to file '%s'. Code: %lu\n",
                    SUMMARY_FILE, GetLastError());
            return;
        }
    }
    printf("INFO\tSuccessfully wrote to '%s'.\n", SUMMARY_FILE);

    CloseHandle(handle);
}

void create_files(void) {
    short int index = 0;
    enumerate_registry_metadata(HKEY_LOCAL_MACHINE, FILES[index++]);
    enumerate_registry_metadata(HKEY_CURRENT_CONFIG, FILES[index++]);
    enumerate_registry_metadata(HKEY_CLASSES_ROOT, FILES[index++]);
    enumerate_registry_metadata(HKEY_CURRENT_USER, FILES[index++]);
    enumerate_registry_metadata(HKEY_USERS, FILES[index]);
}

void trigger_stage_two_protocol(LPSTR bridge, LPSTR bridge_back) {
    printf("INFO\t==== Stage Two initiated. ====\n");
    printf("INFO\tCurrent path: %s\n", bridge);

    /* Create a file corresponding to a hive in the Windows Registry */
    create_files();

    /* Write the absolute path of each file to the summary file */
    write_summary(bridge);

    if (append_to_path(bridge_back, bridge) == -1) {
        fprintf(stderr, "ERROR\tUnable to append string.\n");
        return;
    }

    if (append_to_path(bridge_back, SUMMARY_FILE) == -1) {
        fprintf(stderr, "ERROR\tUnable to append string.\n");
        return;
    }
}