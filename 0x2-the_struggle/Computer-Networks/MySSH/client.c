/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pty.h>
#include <sys/wait.h>

/* Custom headers */
#include "include/config.h"
#include "include/auxiliary.h"
#include "include/OpenSSL_helper.h"

/* Function prototypes */
static int establish_connection(const char *ip_address, const char *port);
static void handle_remote_connection(SSL_CTX *ctx, int sock);

/* Main driver */
int main(int argc, char *argv[])
{
    SSL_CTX *ctx;
    const SSL_METHOD *method;

    if (argc != 3) {
        printf("ＵＳＡＧＥ: %s <IP ADDRESS> <PORT>\n", argv[0]);
        handle_error_hard("invalid number of command-line arguments received");
    }

    /* Here starts configuring OpenSSL */
    _OpenSSL_initialization();

    if ((method = TLS_client_method()) == NULL) {
        _OpenSSL_handle_error("unable to set-up encryption protocol");
    }

    if ((ctx = SSL_CTX_new(method)) == NULL) {
        _OpenSSL_handle_error("unable to create SSL CTX object");
    }


    int sock_fd = establish_connection(argv[1], argv[2]);
    handle_remote_connection(ctx, sock_fd);

    _OpenSSL_cleanup(ctx);
    return 0;
}

int establish_connection(const char *ip_address, const char *port) {
    int sock, addrlen;
    struct sockaddr_in s_conn;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        handle_error_hard("unable to create socket");
    }

    addrlen = sizeof(struct sockaddr_in);
    memset(&s_conn, 0, addrlen);

    s_conn.sin_family = AF_INET;
    s_conn.sin_port   = htons(atoi(port));
    if (inet_pton(AF_INET, ip_address, &(s_conn.sin_addr)) <= 0) {
      handle_error_hard("invalid IPv4 address received");
    }

    if (connect(sock, (struct sockaddr *) &s_conn, sizeof s_conn) == -1) {
        handle_error_hard("unable to connect to remote server");
    }

    return sock;
}

void handle_remote_connection(SSL_CTX *ctx, int sock) {
    int nbytes, fdmax;
    fd_set master, read_fds;
    char buffer[MAX_BUFFER_SIZE];

    /* Set some attributes on the CLIENT's terminal */
    set_terminal_attributes(STDIN_FILENO);

    SSL *ssl = SSL_new (ctx);           /* get new SSL state with context */
    SSL_set_fd(ssl, sock);      /* set connection socket to SSL state */

    if (SSL_connect(ssl) <= 0) {
        _OpenSSL_handle_error("SSL-connecting failed");
    }

    _OpenSSL_get_certificates(ssl);

    /* Clear the set of descriptors meant to be used for reading and the master
     * set.
     */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* Add standard input and the socket descriptor to the master set */
    FD_SET(STDIN_FILENO, &master);
    FD_SET(sock, &master);

    /* Set the descriptor with the maximum value */
    fdmax = sock;

    for (;;) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            handle_error_hard("in select");
        }

        for (int i = 0; i <= fdmax; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == STDIN_FILENO) {
                    memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));

                    if (read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE * sizeof(char)) == -1) {
                        handle_error_hard("cannot read from standard input");
                    }

                    /* Send it! */
                    if (SSL_write(ssl, buffer, strlen(buffer) + 1) == -1) {
                        handle_error_hard("cannot write to remote server");
                    }
                }else if (i == sock) {
                    memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));

                    if ((nbytes = SSL_read(ssl, buffer, MAX_BUFFER_SIZE)) == -1) {
                        handle_error_hard("cannot read from remote server");
                    }else if (nbytes == 0) {
                        /* Close the connection and exit the function */
                        SSL_free (ssl);        /* release connection state */
                        close(sock);
                        return;
                    }else{
                        /* Bypass the buffering of printf() */
                        if (write(STDOUT_FILENO, buffer, strlen(buffer) + 1) == -1) {
                            handle_error_hard("cannot write to standard output");
                        }
                    }   /* END of else */
                }   /* END of else-if */
            }   /* END of if */
        }   /* END of for */
    }   /* END of the infite, processing loop (for) */
}   /* END of handle_remote_connection() */
