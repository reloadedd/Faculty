#include "../include/OpenSSL_helper.h"

void _OpenSSL_initialization() {
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_library_init();
}

void _OpenSSL_cleanup(SSL_CTX *ctx) {
    ERR_free_strings(); /* free memory from SSL_load_error_strings */
    EVP_cleanup();      /* free memory from OpenSSL_add_all_algorithms */
    SSL_CTX_free(ctx);
}

void _OpenSSL_handle_error(const char *err) {
    perror(err);
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

void _OpenSSL_get_certificates(SSL *ssl) {
    /* Get a pointer to a certificate the peer (client / server) presented
     * -- Note: It may be not available (often in case of the peer being a client).
     * A SSL/TLS server will always send a certificate. A client may not send one,
     * but must send if explicitly requested by the server.
     */
    X509 *cert = SSL_get_peer_certificate (ssl);
    if (cert) {
        char *line;
        printf ("Server certificates:\n");
        line = X509_NAME_oneline (X509_get_subject_name (cert), 0, 0);
        printf ("Subject: %s\n", line);
        free (line);
        line = X509_NAME_oneline (X509_get_issuer_name (cert), 0, 0);
        printf ("Issuer: %s\n", line);
        free (line);

        /* Explictly free the X509 object */
        X509_free (cert);
    }else{
        printf("No available certificates\n");
    }
}
