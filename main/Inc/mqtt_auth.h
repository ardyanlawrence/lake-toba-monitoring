#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "mbedtls/md.h"

void build_username(char uid[13], char *username);
void build_password(char uid[13], char *password);
void utils_bin2hex(char *const hex, const size_t hex_maxlen, const unsigned char *const bin, const size_t bin_len);