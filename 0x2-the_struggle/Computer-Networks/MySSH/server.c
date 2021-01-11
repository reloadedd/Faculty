/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */
#define _GNU_SOURCE /* sigaction, sigemptyset, sigprocmask and their friends */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <pty.h>
#include <fcntl.h>

/* Custom headers */
#include "include/config.h"
#include "include/auxiliary.h"
#include "include/OpenSSL_helper.h"
#include "include/cryptography.h"

/* Global variables */
volatile sig_atomic_t G_STILL_ALIVE = TRUE;

/* Function prototypes */
static void sigchld_handler_child(int sig);
static void sigchld_handler_parent(int sig);

#ifdef USE_ONE_TIME_PASSWORD
static int exec_conn_handler(SSL_CTX *ctx, int client,
    struct sockaddr_in peer_addr, const char *otp);
#else
static int exec_conn_handler(SSL_CTX *ctx, int client, struct sockaddr_in peer_addr);
#endif

/* Main driver */
int main(int argc, char *argv[]) {
    SSL_CTX *ctx;
    int sck, client;
    pid_t child_pid;
    sigset_t intmask;
    socklen_t addrlen;
    struct sigaction sa;
    const SSL_METHOD *method;
    const char *otp;
    struct sockaddr_in this, peer_addr;

    /* Zero-out the sockaddr_in structs */
    memset(&this, 0, sizeof this);
    memset(&peer_addr, 0, sizeof peer_addr);

    /* Set-up the server struct used to for binding */
    this.sin_port        = htons(LISTENING_PORT);
    this.sin_family      = AF_INET;
    this.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((sck = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        handle_error_hard("unable to create socket");
    }

    int yes = 1;
    if (setsockopt(sck, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        handle_error_hard("unable to set socket option");
    }

    addrlen = sizeof(struct sockaddr_in);
    if (bind(sck, (struct sockaddr *) &this, addrlen) == -1) {
        handle_error_hard("unable to bind socket");
    }

    if (listen(sck, LISTEN_MAX_BACKLOG) == -1) {
        handle_error_hard("unable to mark the socket as passive in listen");
    }

    /* Display the banner that will be seen when the program will be run */
    display_banner();

    /* Here starts configuring OpenSSL */
    _OpenSSL_initialization();

    if ((method = TLS_server_method()) == NULL) {
        _OpenSSL_handle_error("unable to set-up encryption protocol");
    }

    if ((ctx = SSL_CTX_new(method)) == NULL) {
        _OpenSSL_handle_error("unable to create SSL CTX object");
    }

    /* Set the local certificate from file */
    if (SSL_CTX_use_certificate_file (ctx, CERTIFICATE, SSL_FILETYPE_PEM) <= 0) {
        _OpenSSL_handle_error("unable to load certificate file");
    }

    /* Set the private key from file (may be the same file as previous) */
    if (SSL_CTX_use_PrivateKey_file (ctx, CERTIFICATE, SSL_FILETYPE_PEM) <= 0) {
        _OpenSSL_handle_error("unable to use private key from certificate file");
    }

    /* Verify private key */
    if (!SSL_CTX_check_private_key (ctx)) {
        _OpenSSL_handle_error("verification of private key failed");
    }

    /* Set up the signal handler for parent process in order to reap the zombie
     * processes. The SA_RESTART flag resumes the library function which was
     * interrupted after the signal handler is done.
     */
    sa.sa_handler = sigchld_handler_parent;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        handle_error_hard("unable to set signal handler for SIGCHLD");
    }

    if ((sigemptyset(&intmask) == -1) || (sigaddset(&intmask, SIGCHLD) == -1)) {
       handle_error_hard("failed to initialize signal mask");
    }

    if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1) {
        handle_error_hard("unable to block signal in parent");
    }

    while((client = accept(sck, (struct sockaddr *) &peer_addr, &addrlen)) != -1) {
#ifdef USE_ONE_TIME_PASSWORD
        printf("%s:【 %s 】\n", OTP, (otp = one_time_password()));
#endif

        if ((child_pid = fork()) == -1) {
            handle_error_hard("at fork");
        }

        if (child_pid == 0) {
            struct sigaction sa_chld;
            sa_chld.sa_handler = sigchld_handler_child;
            sigemptyset(&sa_chld.sa_mask);
            sa_chld.sa_flags = SA_RESTART;

            if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
                handle_error_hard("unable to set signal handler for SIGCHLD");
            }

            if (sigprocmask(SIG_UNBLOCK, &intmask, NULL) == -1) {
                handle_error_hard("unable to block signal in child");
            }
            /* The child process doesn't need the listener */
            close(sck);
#ifdef USE_ONE_TIME_PASSWORD
            exec_conn_handler(ctx, client, peer_addr, otp);
#else
            exec_conn_handler(ctx, client, peer_addr);
#endif
        }

        if (sigprocmask(SIG_UNBLOCK, &intmask, NULL) == -1) {
            handle_error_hard("unable to unblock signal in parent");
        }

        /* Parent doesn't need the socket descriptor used to communicate with
         * the client, that's the job of the child process. If the socket
         * descriptor for the client isn't closed by the parent, the client
         * will never receive EOF when doing read on the socket, thus it won't
         * close the connection and will wait forever (until the server process
         * is terminated).
         */
        close(client);
    }

    if (client == -1) {
        handle_error_hard("unable to accept incoming connection");
    }

    _OpenSSL_cleanup(ctx);
    close(sck);

    return 0;
}

void sigchld_handler_parent(int sig) {
    /* Credits: Beej's Guide to Network Programming, Chapter: Client-Server
     * Background, page 31.
     */
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);

    /* waitpid() might overwrite errno, so we save and restore it */
    errno = saved_errno;
}

void sigchld_handler_child(int sig) {
    /* Credits: Beej's Guide to Network Programming, Chapter: Client-Server
     * Background, page 31.
     */
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);

    G_STILL_ALIVE = FALSE;
    /* waitpid() might overwrite errno, so we save and restore it */
    errno = saved_errno;
}

#ifdef USE_ONE_TIME_PASSWORD
int exec_conn_handler(SSL_CTX *ctx, int client, struct sockaddr_in peer_addr,
    const char *otp) {
#else
int exec_conn_handler(SSL_CTX *ctx, int client, struct sockaddr_in peer_addr) {
#endif
    pid_t child_pty;
    fd_set master, read_fds;
    char buffer[MAX_BUFFER_SIZE];
    int terminal_fd, nbytes, fdmax;

    SSL *ssl = SSL_new (ctx);           /* get new SSL state with context */
    SSL_set_fd (ssl, client);      /* set connection socket to SSL state */

    /* do SSL-protocol accept */
    if (SSL_accept(ssl) <= 0) {
        _OpenSSL_handle_error("SSL-accepting failed");
    }

    // _OpenSSL_get_certificates(ssl);

#ifdef USE_ONE_TIME_PASSWORD
    memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));

    if ((nbytes = SSL_read(ssl, buffer, MAX_BUFFER_SIZE)) == -1) {
        handle_error_hard("cannot read One Time Password from remote server");
    }

    if (strcmp(otp, buffer) != 0) {
        log_remote_connection(R_LOG_FAILED_OTP, getpid(), peer_addr);
        close(client);
        SSL_free(ssl);
        exit(EXIT_SUCCESS);
    }
#endif

    if ((child_pty = forkpty(&terminal_fd, NULL, NULL, NULL)) == -1) {
        handle_error_hard("at fork");
    }

    if (child_pty == 0) {
        /* Don't pass the client descriptor to the forkpty'ed process,
         * because it doesn't need it.
         */
        close(client);
        execl(SHELL, SHELL, (char *) NULL);
        handle_error_hard("the execl(3) call failed");
    }

    /* Log the remote connections from clients to stdout */
    log_remote_connection(R_LOG_CONNECTION, getpid(), peer_addr);
#ifdef DEBUG
    printf("%s: Attached pseudo-terminal (PID: %d) to client with PID: %d.\n",
            PS2, child_pty, getpid());
#endif

    /* Clear the set of descriptors meant to be used for reading and the master
     * set.
     */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* Add terminal descriptor and the socket descriptor to the master set */
    FD_SET(terminal_fd, &master);
    FD_SET(client, &master);

    /* Set the descriptor with the maximum value */
    fdmax = client;

    while (G_STILL_ALIVE == TRUE) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            handle_error_hard("in select");
        }

        for (int i = 0; i <= fdmax; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                /* Zero out the buffer */
                memset(buffer, '\0', MAX_BUFFER_SIZE * sizeof(char));

                if (i == terminal_fd) {
                    /* If any I/O operation fails and the master side of the
                     * pseudoterminal is not closed, then raise an error and
                     * terminate the program.
                     */
                    if (read(terminal_fd, buffer, MAX_BUFFER_SIZE) == -1 &&
                            fcntl(terminal_fd, F_GETFD) > 0) {
                        handle_error_hard("cannot read from pseudo-terminal");
                    }

                    /* Encrypt using SSL the message and send it to the client */
                    if (SSL_write(ssl, buffer, strlen(buffer)) == -1 &&
                            fcntl(terminal_fd, F_GETFD) > 0) {
                        handle_error_hard("cannot write to remote client");
                    }
                }else if (i == client) {
                    /* Read and decrypt using SSL the message received */
                    if ((nbytes = SSL_read(ssl, buffer, MAX_BUFFER_SIZE)) == -1) {
                        handle_error_soft("cannot read from remote client");
                    }else if (nbytes == 0) {
                        /* In the next iteration of while, close the connection
                         * and exit the function.
                         */
                         G_STILL_ALIVE = FALSE;
                         break;
                    }else{
                        // if (iscntrl(buffer[0])) {
                        //   printf("%d\n", buffer[0]);
                        // } else {
                        //   printf("%d ('%c')\n", buffer[0], buffer[0]);
                        // }

                        if (write(terminal_fd, buffer, strlen(buffer)) == -1) {
                            handle_error_hard("cannot write to pseudo-terminal");
                        }
                    }   /* END of else */
                }   /* END of else-if */
            }   /* END of if */
        }   /* END of for */
    }   /* END of the processing loop (while) */

    /* Release SSL state */
    SSL_free(ssl);

    /* Close the client socket descriptor */
    close(client);

    /* Log disconnected clients */
    log_remote_connection(R_LOG_CLIENT_DISCONNECTED, getpid(), peer_addr);

#ifdef DEBUG
    printf("%s: Pseudo-terminal attached at PID: %d, terminated.\n", PS2, child_pty);
    printf("%s: Client with assigned PID: %d, disconnected.\n", PS2, getpid());
#endif

    /* Terminate the forked process successfully */
    exit(EXIT_SUCCESS);
}
