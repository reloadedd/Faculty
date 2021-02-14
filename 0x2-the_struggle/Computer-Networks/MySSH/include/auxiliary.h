/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */

/* This header file contains auxiliary functions (for both the client and server)
 * for general use.
 */
#ifndef __INCLUDE_AUXILIARY_H_
#define __INCLUDE_AUXILIARY_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "config.h"

extern void handle_error_soft(const char *msg);
extern void handle_error_hard(const char *msg);
extern void display_banner();
extern void log_remote_connection(int reason, pid_t pid,
    struct sockaddr_in r_conn_details);
extern void set_terminal_attributes(int fd);
extern void clear_screen(int screen);

#endif /* __INCLUDE_AUXILIARY_H_ */
