/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */
#ifndef __INCLUDE_OPENSSL_HELPER_H_
#define __INCLUDE_OPENSSL_HELPER_H_

#include <stdio.h>
#include <stdlib.h>

/* OpenSSL-specific headers */
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/x509.h>

extern void _OpenSSL_initialization();
extern void _OpenSSL_cleanup(SSL_CTX *ctx);
extern void _OpenSSL_handle_error(const char *err);
extern void _OpenSSL_get_certificates(SSL *ssl);

#endif /* __INCLUDE_OPENSSL_HELPER_H_ */
