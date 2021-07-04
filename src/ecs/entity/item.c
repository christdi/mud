#include <string.h>

#include "mud/data/hash_table.h"
#include "mud/ecs/description.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/entity/item.h"
#include "mud/ecs/location.h"
#include "mud/game.h"
#include "mud/util/muduuid.h"

/**
 * Creates and registers the components necessary to represent a item.
 *
 * This function takes the following parameters:
 *   game - a pointer to a game struct containing components
 *   name - the name to use for the new location
 *   description - the description to use for the new item
 *
 * Returns a pointer to an entity struct representing the new item
**/
entity_t* new_item(game_t* game, char* name, char* description) {
  entity_t* item = create_entity_t();
  generate_uuid(item->uuid, UUID_SIZE);
  hash_table_insert(game->entities, item->uuid, item);

  description_t* item_description = create_description_t();
  strncpy(item_description->uuid, item->uuid, UUID_SIZE);
  item_description->name = strdup(name);
  item_description->description = strdup(description);
  register_description(game->components, item_description);

  location_t* location = create_location_t();
  strncpy(location->uuid, item->uuid, UUID_SIZE);
  register_location(game->components, location);

  return item;
}
