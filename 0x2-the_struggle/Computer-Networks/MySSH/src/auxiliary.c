/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */
#include "../include/auxiliary.h"

void handle_error_soft(const char *msg) {
    fprintf(stderr, "ＥＲＲＯＲ: %s. ", msg);
    fflush(stdout);
    if (errno != 0) {
        perror("Reason");
    }else{
        printf("\n");
    }
}

void handle_error_hard(const char *msg) {
    fprintf(stderr, "ＦＡＴＡＬ ＥＲＲＯＲ: %s. ", msg);
    fflush(stdout);
    if (errno != 0) {
        perror("Reason");
    }else{
        printf("\n");
    }
    exit(EXIT_FAILURE);
}

void display_banner() {
    printf(
    "88b           d88                ad88888ba    ad88888ba   88        88\n"
    "888b         d888               d8\"     \"8b  d8\"     \"8b  88        88\n"
    "88`8b       d8'88               Y8,          Y8,          88        88\n"
    "88 `8b     d8' 88  8b       d8  `Y8aaaaa,    `Y8aaaaa,    88aaaaaaaa88\n"
    "88  `8b   d8'  88  `8b     d8'    `\"\"\"\"\"8b,    `\"\"\"\"\"8b,  88\"\"\"\"\"\"\"\"88\n"
    "88   `8b d8'   88   `8b   d8'           `8b          `8b  88        88\n"
    "88    `888'    88    `8b,d8'    Y8a     a8P  Y8a     a8P  88        88\n"
    "88     `8'     88      Y88'      \"Y88888P\"    \"Y88888P\"   88        88\n"
    "                       d8'                                            \n"
    "                      d8'                                             \n"
    "\t\tＭＡＤＥ ＷＩＴＨ ❤️ ＢＹ ＲＯＳＣＡ ＩＯＮＵＴ\n\n"
    );
#ifdef DEBUG
    printf("%s: Version: %s |+| Listening port: %d\n", PS2, VERSION,
            LISTENING_PORT);
#endif
}

void log_remote_connection(pid_t pid, struct sockaddr_in r_conn_details) {
    time_t t = time(NULL);
    struct tm _tm = *localtime(&t);
    char ipv4[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &(r_conn_details.sin_addr), ipv4, INET_ADDRSTRLEN) == NULL) {
      handle_error_soft("cannot decode IPv4 address received in logging function");
    }

    printf("[ %d-%02d-%02d %02d:%02d:%02d ] (PID: %d) Got connection from IP: "
            "%s, PORT: %d\n", _tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday,
            _tm.tm_hour, _tm.tm_min, _tm.tm_sec, pid, ipv4,
            ntohs(r_conn_details.sin_port));
    fflush(stdout);
}

void set_terminal_attributes(int fd) {
    struct termios terminal;

    /* Get terminal attributes associated with the file descriptor specified as
     * the first parameter and stores them in the termios structure passed as
     * second argument by reference.
     */
    if (tcgetattr(fd, &terminal) == -1) {
        /* Won't exit, but warn the client */
        handle_error_soft("unable to get terminal attributes");
    }

    /* Turn off some flags, their meaning will be explained below. The
     * explanations from below explain what does that flag DISABLED.
     * -- ICANON: Disables canonical mode. Input will be available character by
     *    character.
     * -- ECHO: Disables echoing. Do not display what have been just typed.
     * -- ECHOE:
     * -- ISIG: Disables signals associated with the following characters:
     *    == INTR (CTRL + C)
     *    == QUIT (CTRL + \)
     *    == SUSP (CTRL + Z)
     *    == DSUSP (Delayed Suspend Character will send SIGTSP signal once the
     *       input is flushed -- not bound to any key combinations from my
     *       knowledge)
     */
    terminal.c_lflag &= ~(ICANON | ECHO | ISIG);

    /* Set the terminal attributes (what was before + the tweaked ones) for the
     * specified file descriptor
     */
    if (tcsetattr(fd, TCSANOW, &terminal) == -1) {
        /* Won't exit, but warn the client */
        handle_error_soft("unable to set terminal attributes");
    }
}
