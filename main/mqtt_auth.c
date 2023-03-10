#include <stdlib.h>
#include <stdint.h>
#include "mqtt_auth.h"

const char PREFIX[] = "mqttuid";
const char SECRET_PREFIX[] = "sensormqttuidsecret";

void build_username(char uid[13], char *username) {
    sprintf(username, "%s_%s", PREFIX, uid);
}

void build_password(char uid[13], char *password) {
    unsigned char hmacResult[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    char message[21];
    char secret[27];

    sprintf(message, "%s_%s", PREFIX, uid);
    sprintf(secret, "%s_%s", SECRET_PREFIX, uid);

    const size_t payloadLength = strlen(message);
    const size_t keyLength = strlen(secret);      

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *) secret, keyLength);
    mbedtls_md_hmac_update(&ctx, (const unsigned char *) message, payloadLength);
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);
 
    char hash_str[sizeof(hmacResult) * 2 + 1];
    utils_bin2hex(hash_str, sizeof(hash_str), hmacResult, sizeof(hmacResult));
    sprintf(password, "%s", hash_str);
}

static inline char to_hex_digit(unsigned val)
{
    return (val < 10) ? ('0' + val) : ('a' + val - 10);
}


void utils_bin2hex(char *const hex, const size_t hex_maxlen, const unsigned char *const bin, const size_t bin_len)
{
    assert(bin_len < SIZE_MAX / 2);
    assert(hex_maxlen > bin_len * 2U);
    assert(hex);
    assert(bin);

    int i;
    for (i = 0; i < bin_len; i++) {
        hex[2*i] = to_hex_digit(bin[i] >> 4);
        hex[2*i + 1] = to_hex_digit(bin[i] & 0xf);
    }
    hex[i * 2U] = 0U;
}