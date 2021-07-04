#ifndef _MUD_UUID_H_
#define _MUD_UUID_H_

#include <stddef.h>

/**
 * Definitions
**/
#define UUID_SIZE 37

/**
 * Function prototypes
**/
void generate_uuid(char* dest, size_t size);

#endif