#include <string.h>

#include "mud/ecs/entity/location.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/description.h"
#include "mud/ecs/location.h"
#include "mud/data/hash_table.h"
#include "mud/util/muduuid.h"
#include "mud/game.h"


/**
 * Creates and registers the components necessary to represent a location.
 *
 * This function takes the following parameters:
 *   game - a pointer to a game struct containing components
 *   name - the name to use for the new location
 *   description - the description to use for the new location
 *
 * Returns a pointer to an entity struct representing the new location
**/
entity_t * new_location(game_t * game, char * name, char * description) {
	entity_t * location = create_entity_t();
	generate_uuid(location->uuid, UUID_SIZE);
	hash_table_insert(game->entities, location->uuid, location);

	description_t * location_description = create_description_t();
	strncpy(location_description->uuid, location->uuid, UUID_SIZE);
	location_description->name = strdup(name);
	location_description->description = strdup(description);
	register_description(game->components, location_description);

	location_t * parent_location = create_location_t();
	strncpy(parent_location->uuid, location->uuid, UUID_SIZE);
	register_location(game->components, parent_location);

	return location;
}