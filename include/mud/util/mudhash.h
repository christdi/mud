#ifndef _MUD_HASH_H_
#define _MUD_HASH_H_

/**
 * Definitions
**/
#define SHA256_SIZE 64
#define MAX_SHA256_STRING_LENGTH 128

/**
 * Function prototypes
**/
void string_to_sha256(char* input, char* output);

#endif
