#include <assert.h>
#include <stdlib.h>

#include "mud/ecs/components.h"
#include "mud/data/hash_table/hash_table.h"
#include "mud/log.h"


/**
 * Allocates and initialises a components_t struct.
 *
 * Returns the allocated struct.
**/
components_t * create_components_t() {
	components_t * components = calloc(1, sizeof * components);

	components->character_details = create_hash_table_t();

	return components;
}


/**
 * Frees an allocated component_t struct.
**/
void free_components_t(components_t * components) {
	assert(components);

	free_hash_table_t(components->character_details);

	free(components);
	components = NULL;
}
