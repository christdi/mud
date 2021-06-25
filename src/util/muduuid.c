#include "mud/util/muduuid.h"
#include "mud/log.h"

#include <assert.h>
#include <uuid/uuid.h>

/**
 * Genereate a 36 character alphanumeric lower case UUID and store it in the buffer pointed
 * to by dest.  The buffer must be at least UUID_SIZE in size or the UUID will overrun the
 * allocated space.
**/
void generate_uuid(char* dest, size_t size) {
  assert(dest);

  if (size < UUID_SIZE) {
    zlog_error(gc, "Error when generating uuid, destination buffer supplied was not big enough.");

    return;
  }

  uuid_t uuid_bin;
  uuid_generate_random(uuid_bin);
  uuid_unparse_lower(uuid_bin, dest);
}