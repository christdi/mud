#include <assert.h>
#include <stdlib.h>

#include "mud/data/hash_table.h"
#include "mud/ecs/components.h"
#include "mud/log.h"

/**
 * Allocates and initialises a components_t struct.
 *
 * Returns the allocated struct.
**/
components_t* create_components_t() {
  components_t* components = calloc(1, sizeof *components);

  components->description = create_hash_table_t();
  components->location = create_hash_table_t();

  return components;
}

/**
 * Frees an allocated component_t struct.
**/
void free_components_t(components_t* components) {
  assert(components);

  if (components->description) {
    free_hash_table_t(components->description);
    components->description = NULL;
  }

  if (components->location) {
    free_hash_table_t(components->location);
    components->location = NULL;
  }

  free(components);
  components = NULL;
}
