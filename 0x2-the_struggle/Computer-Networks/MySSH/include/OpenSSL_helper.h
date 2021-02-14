/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */

/* This header file contains functions that interact with OpenSSL API and are
 * prefixed with an underscore (_) to be distinguished from the functions of
 * the OpenSSL API.
 */
#ifndef __INCLUDE_OPENSSL_HELPER_H_
#define __INCLUDE_OPENSSL_HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* OpenSSL-specific headers */
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>

/* Custom headers */
#include "auxiliary.h"

extern void _OpenSSL_initialization();
extern void _OpenSSL_cleanup(SSL_CTX *ctx);
extern void _OpenSSL_handle_error(const char *err);
extern void _OpenSSL_get_certificate(SSL *ssl);

#endif /* __INCLUDE_OPENSSL_HELPER_H_ */
