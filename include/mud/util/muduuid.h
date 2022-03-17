#ifndef MUD_UTIL_MUDUUID_H
#define MUD_UTIL_MUDUUID_H

/**
 * Structs
 **/
typedef struct mud_uuid {
  char raw[37];
} mud_uuid_t;

/**
 * Function prototypes
 **/
mud_uuid_t new_uuid();
const char* uuid_str(const mud_uuid_t* uuid);
mud_uuid_t str_uuid(const char* data);

#endif
