#include <assert.h>
#include <uuid/uuid.h>

#include "bsd/string.h"

#include "mud/util/muduuid.h"

#define UUID_SIZE 37

/**
 * Creates a new mud_uuid_t.
 * 
 * Returns a copy of the generated mud_uuid_t.
**/
mud_uuid_t new_uuid() {
  mud_uuid_t uuid;

  uuid_t uuid_bin;
  uuid_generate_random(uuid_bin);
  uuid_unparse_lower(uuid_bin, uuid.raw);

  return uuid;
}

/**
 * Parameters
 *   uuid - UUID to get string from
 * 
 * Returns a string representation of a UUID.
**/
const char* uuid_str(const mud_uuid_t* uuid) {
  assert(uuid);

  return uuid->raw;  
}


/**
 * Returns a mud_uuit_t populated with a UUID.
 * 
 * Parameters
 *   data -UUID as string to be copied into structure
 * 
 * Returns a copy of the generated mud_uuid_t.
**/
mud_uuid_t str_uuid(const char* data) {
  assert(data);
  
  mud_uuid_t uuid;

  strlcpy(uuid.raw, data, UUID_SIZE);

  return uuid;
}
