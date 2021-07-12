#include <assert.h>
#include <stdlib.h>

#include "mud/ecs/component/description.h"
#include "mud/ecs/component/location.h"
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
  components->description->deallocator = deallocate_description_t;

  components->location = create_hash_table_t();
  components->location->deallocator = deallocate_location_t;

  return components;
}

/**
 * Frees an allocated component_t struct.
**/
void free_components_t(components_t* components) {
  assert(components);

  if (components->description != NULL) {
    free_hash_table_t(components->description);
  }

  if (components->location != NULL) {
    free_hash_table_t(components->location);
  }

  free(components);
}
