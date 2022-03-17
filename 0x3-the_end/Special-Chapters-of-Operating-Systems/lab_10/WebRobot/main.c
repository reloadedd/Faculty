#include <stdio.h>
#include "lib/utils.h"


int _tmain(int argc, TCHAR *argv[]) {
    createAndSetWorkingDirectory(ROOT_DIR);

    /* Retrieve the file from the remote server which will be saved as `config.txt`.
     * Also,
     *      len("/config/310910401ESL191074") -> 26 + 1
     */
    TCHAR *configRemotePath = malloc(sizeof(TCHAR) * 27);
    snprintf(configRemotePath, 27, "/config/%s", NR_MATRICOL);
    getRequestAndSaveResponse(REMOTE_URL, configRemotePath, CONFIG_FILE);

    /* Retrieve the file from the remote server which will be saved as `executabile.txt`.
     * Also,
     *      len("/executabile/310910401ESL191074") -> 31 + 1
     */
    TCHAR *executablesRemotePath = malloc(sizeof(TCHAR) * 32);
    snprintf(executablesRemotePath, 32, "/executabile/%s", NR_MATRICOL);
    getRequestAndSaveResponse(REMOTE_URL, executablesRemotePath, EXECUTABLES_FILE);

    /* Create the sub-folders */
    createSubFolders(CONFIG_FILE);

    TCHAR *path = malloc((4 + strlen(EXECUTABLES_FILE)) * sizeof(TCHAR));
    snprintf(path, (4 + strlen(EXECUTABLES_FILE)) * sizeof(TCHAR), "..\\%s", EXECUTABLES_FILE);
    downloadAndSaveFiles(path);

    free(path);
    free(configRemotePath);
    free(executablesRemotePath);

    return 0;
}
