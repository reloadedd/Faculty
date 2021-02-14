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
    if (errno != 0) {
        perror(err);        
    }
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

void _OpenSSL_get_certificate(SSL *ssl) {
    /* Get a pointer to a certificate the peer (client / server) presented
     * -- Note: It may be not available (often in case of the peer being a client).
     * A SSL/TLS server will always send a certificate. A client may not send one,
     * but must send if explicitly requested by the server.
     */
    char *line;
    X509 *cert = SSL_get_peer_certificate(ssl);

    if (cert != NULL) {
        printf("---=== Server certificate ===---\n");
        if ((line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)) == NULL) {
            handle_error_soft("unable to get subject name of certificate");
        }else{
            printf("├── Subject: %s\n", line);
            free (line);
        }
        if ((line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)) == NULL) {
            handle_error_soft("unable to get issuer name of certificate");
        }else{
            printf("├── Issuer: %s\n", line);
            free(line);
        }

        /* Credits: https://stackoverflow.com/a/17087682/10604490 */
        ASN1_INTEGER *serial = X509_get_serialNumber(cert);
        BIGNUM *bnser = ASN1_INTEGER_to_BN(serial, NULL);
        if ((line = BN_bn2hex(bnser)) == NULL) {
            handle_error_soft("unable to get serial number of certificate");
        }else{
            printf("├── Serial number: %s\n", line);

            /* Free according to documentation */
            OPENSSL_free(line);
        }

        /* Explictly free the X509 object */
        X509_free(cert);
    }else{
        printf("No available certificates\n");
    }
}
