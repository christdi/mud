#include "mud/util/muduuid.h"

#include <assert.h>
#include <uuid/uuid.h>

/**
 * Genereate a 36 character alphanumeric lower case UUID and store it in the buffer pointed
 * to by dest.  The buffer must be at least UUID_SIZE in size or the UUID will overrun the
 * allocated space.
**/
void generate_uuid(char * dest) {
	assert(dest);

	uuid_t uuid_bin;
	uuid_generate_random(uuid_bin);
	uuid_unparse_lower(uuid_bin, dest);
}