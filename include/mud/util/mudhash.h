#ifndef _MUD_HASH_H_
#define _MUD_HASH_H_

/**
 * Definitions
 **/
#define SHA256_HEX_LENGTH 64 /* 64 characters */
#define SHA256_HEX_SIZE 65 /* 64 characters + null terminator */
#define MAX_SHA256_INPUT_LENGTH 128

/**
 * Function prototypes
 **/
void mudhash_sha256(const char* input, char* output);

#endif
