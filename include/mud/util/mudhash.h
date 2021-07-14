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
void string_to_sha256(char* input, char* output);

#endif
