/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */
#ifndef __INCLUDE_CRYPTOGRAPHY_H_
#define __INCLUDE_CRYPTOGRAPHY_H_

#include <stdlib.h> /* for arc4random_buf, need to link with -lbsd */
#include <stdio.h>

#define ONE_TIME_PASSWORD_LENGTH    35

/* Get rid of warning */
extern void arc4random_buf(void *buf, size_t nbytes);

extern const char *one_time_password();

#endif /* __INCLUDE_CRYPTOGRAPHY_H_ */
