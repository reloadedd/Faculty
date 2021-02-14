/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

/* Custom headers */
#include "include/config.h"
#include "include/auxiliary.h"
#include "include/OpenSSL_helper.h"

/* Function prototypes */
static int establish_connection(const char *ip_address, const char *port);
static void handle_remote_connection(SSL_CTX *ctx, int sock);
#ifdef USE_ONE_TIME_PASSWORD
static void authenticate(SSL *ssl, int sock);
#endif

/* Main driver */
int main(int argc, char *argv[])
{
    SSL_CTX *ctx;
    const SSL_METHOD *method;

    if (argc != 3) {
        printf("ＵＳＡＧＥ: %s <IP ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_SUCCESS);
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
        close(sock);
        handle_error_hard("invalid IPv4 address received");
    }

    if (connect(sock, (struct sockaddr *) &s_conn, sizeof s_conn) == -1) {
        close(sock);
        handle_error_hard("unable to connect to remote server");
    }

    return sock;
}

#ifdef USE_ONE_TIME_PASSWORD
void authenticate(SSL *ssl, int sock) {
    char buffer[MAX_BUFFER_SIZE];
    int nbytes;

    memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));

    printf("The server requires a One Time Password in order to authenticate."
    "\n%s: ", OTP);
    fflush(stdout);

    if (read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE * sizeof(char)) == -1) {
        SSL_free(ssl);        /* release connection state */
        close(sock);
        handle_error_hard("cannot read from standard input");
    }else if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }

    if (SSL_write(ssl, buffer, strlen(buffer) + 1) == -1) {
        SSL_free(ssl);        /* release connection state */
        close(sock);
        handle_error_hard("cannot write to remote server");
    }

    memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));
    if ((nbytes = SSL_read(ssl, buffer, MAX_BUFFER_SIZE)) == -1) {
        SSL_free(ssl);        /* release connection state */
        close(sock);
        handle_error_hard("cannot read from remote server");
    }

    if (nbytes == 0) {
        fprintf(stderr, "Verification of OTP failed because of incorrect OTP\n");
        close(sock);
        SSL_free (ssl);        /* release connection state */
        exit(EXIT_FAILURE);
    }

    /* If we're here, it means that the OTP was correct.
     * Now, let's refresh the screen.
     */
    clear_screen(STDOUT_FILENO);

    /* And then, print out the data received from the server */
    if (write(STDOUT_FILENO, buffer, strlen(buffer) + 1) == -1) {
        SSL_free(ssl);        /* release connection state */
        close(sock);
        handle_error_hard("cannot write to standard output");
    }
}
#endif

void handle_remote_connection(SSL_CTX *ctx, int sock) {
    int nbytes, fdmax;
    fd_set master, read_fds;
    char buffer[MAX_BUFFER_SIZE];

    SSL *ssl = SSL_new(ctx);           /* get new SSL state with context */
    SSL_set_fd(ssl, sock);      /* set connection socket to SSL state */

    if (SSL_connect(ssl) <= 0) {
        _OpenSSL_handle_error("SSL-connecting failed");
    }

    _OpenSSL_get_certificate(ssl);
    printf("Are you sure you want connect to the server having the "
    "aforementioned certificate (yes/no) ? ");
    fflush(stdout);
    memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));

    if (read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE) == -1) {
        SSL_free(ssl);        /* release connection state */
        close(sock);
        handle_error_hard("cannot read from standard input");
    }else if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }

    if (SSL_write(ssl, buffer, strlen(buffer) + 1) == -1) {
        SSL_free(ssl);        /* release connection state */
        close(sock);
        handle_error_hard("cannot write to remote server");
    }
    if ((strcmp(buffer, A_ACCEPT_CERT_YES) != 0
        && strcmp(buffer, A_ACCEPT_CERT_Y) != 0)) {
        exit(EXIT_SUCCESS);
    }

#ifdef USE_ONE_TIME_PASSWORD
    authenticate(ssl, sock);
#endif

    /* Set some attributes on the CLIENT's terminal */
    set_terminal_attributes(STDIN_FILENO);

#ifndef USE_ONE_TIME_PASSWORD
    clear_screen(STDOUT_FILENO);
#endif
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
            SSL_free(ssl);        /* release connection state */
            close(sock);
            handle_error_hard("in select");
        }

        for (int i = 0; i <= fdmax; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                /* Zero out the buffer */
                memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));

                if (i == STDIN_FILENO) {
                    if (read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE * sizeof(char)) == -1) {
                        SSL_free(ssl);        /* release connection state */
                        close(sock);
                        handle_error_hard("cannot read from standard input");
                    }

                    /* Encrypt using SSL the message and send it to the server */
                    if (SSL_write(ssl, buffer, strlen(buffer) + 1) == -1) {
                        SSL_free(ssl);        /* release connection state */
                        close(sock);
                        handle_error_hard("cannot write to remote server");
                    }
                }else if (i == sock) {
                    /* Read and decrypt using SSL the message received */
                    if ((nbytes = SSL_read(ssl, buffer, MAX_BUFFER_SIZE)) == -1) {
                        SSL_free(ssl);        /* release connection state */
                        close(sock);
                        handle_error_hard("cannot read from remote server");
                    }else if (nbytes == 0) {
                        /* Close the connection and exit the function */
                        SSL_free(ssl);        /* release connection state */
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
