/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */

/* This header file contain constants and macros meant to be available
 * globally to the programs (both client and server). Each change made
 * in this file will be reflected on all code which depend on this
 * header file. And most of the code does.
 */
#ifndef __INCLUDE_CONFIG_H_
#define __INCLUDE_CONFIG_H_

/* The version of the project.
 * Format:
 *      -- v<MAJOR>.<MINOR>.<MICRO>r<REVISION NUMBER>
 * Last revision: 10 December 2020
 */
#define VERSION "v1.0.0r1"

/* The port used by the server to listen for incoming connections.
 * 0xc0de is 49374 in decimal notation.
 */
#define LISTENING_PORT 0xc0de

/* No-value define used to enable or disable DEBUG mode.
 * DEBUG mode is meant to aid in the debugging process by providing
 * extra details on the fly (i.e. extra information while the program is
 * running). This extra information is simply displayed on the screen.
 * --- Usage:
 * In order to *enable* DEBUG mode, uncomment the next define.
 * In order to *disable* DEBUG mode, comment the next define.
 */
// #define DEBUG

/* The prompt that will be displayed when taking input from the client */
#define PS1 "ＰＲＯＭＰＴ > "

/* The prompt that will be used when displaying debugging information on the
 * server side.
 */
#define PS2 "ＤＥＢＵＧ"

/* The prompt that will be used when displaying One Time Passwords */
#define OTP "ＯＴＰ"

/* The maximum length to which the queue of pending connections for the
 * listening socket may grow.
 */
#define LISTEN_MAX_BACKLOG  10

/* The shell to be used for every client that connects to the server. The server
 * will call fork() for each client, and each forked process will call exec with
 * this shell as argument.
 */
#define SHELL   "/bin/bash"

/* The maximum size of a buffer that can be sent at one moment in time over the
 * wire.
 */
#define MAX_BUFFER_SIZE 4096

/* Helper defines to simulate booleans */
#define FALSE   0
#define TRUE    1

/* The certificate file needed by OpenSSL in the server */
#define CERTIFICATE "server_certificate.pem"

/* This macro decides if the server will be free-for-all or users will need to
 * authenticate.
 */
#define USE_ONE_TIME_PASSWORD

/* Reasons checked by the log_remote_connection() function */
#define R_LOG_CONNECTION            0
#define R_LOG_FAILED_OTP            1
#define R_LOG_CLIENT_DISCONNECTED   2
#define R_LOG_CLIENT_DENY_CERT      3

#endif  /* __INCLUDE_CONFIG_H_ */
