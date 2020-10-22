#include <stdlib.h>
#include <uuid/uuid.h>


#include "mud/entity/entity.h"

char * generate_entity_uuid() {
	char * uuid = calloc(1, UUID_SIZE);

	uuid_t uuid_bin;
	uuid_generate_random(uuid_bin);
	uuid_unparse_lower(uuid_bin, uuid);

	return uuid;
}