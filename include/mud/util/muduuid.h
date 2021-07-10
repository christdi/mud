#ifndef MUD_UTIL_MUDUUID_H
#define MUD_UTIL_MUDUUID_H

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
