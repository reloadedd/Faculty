/*  Copyright (c) 2020-2021 Roșca Ionuț <ionut.rosca@info.uaic.ro> */
#include "../include/cryptography.h"

/* Create a One-Time-Password (OTP) by getting high-quality random data and
 * building up a string of length <ONE_TIME_PASSWORD_LENGTH> characters.
 * Spaces are not allowed inside the OTP.
 */
const char *one_time_password() {
    char *random_data = malloc((ONE_TIME_PASSWORD_LENGTH + 1) * sizeof(char));
    arc4random_buf(random_data, ONE_TIME_PASSWORD_LENGTH);
    random_data[ONE_TIME_PASSWORD_LENGTH] = '\0';

    for (int i = 0; i < ONE_TIME_PASSWORD_LENGTH; ++i) {
        if (random_data[i] < 0) {
            random_data[i] -= random_data[i];
        }
        random_data[i] = random_data[i] % 127;

        if ((int)random_data[i] + 32 < 127) {
            random_data[i] = (char) ((int) random_data[i] + 32);
        }

        if (random_data[i] == ' ') {
            random_data[i] = (int) random_data[i] + 1 + i;
        }
    }

    return (const char *) random_data;
}
