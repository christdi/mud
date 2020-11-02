#include <assert.h>
#include <string.h>
#include <uuid/uuid.h>

#include "mud/ecs/entity.h"
#include "mud/ecs/character_details.h"
#include "mud/data/linked_list/linked_list.h"
#include "mud/log/log.h"
#include "mud/game.h"


/**
 * Loads entities from persistence into the game.
 *
 * Takes the following parameters:
 *   game - the game_t struct where the entities should be loaded to
**/
void load_entities(game_t * game) {
	assert(game);

	zlog_info(gc, "Loading entities");

	// TODO: Actually load entities

	char uuid[UUID_SIZE];
	generate_entity_uuid(uuid, UUID_SIZE);

	list_add(game->entities, uuid);

	character_details_t * character_details = create_character_details_t();
	strncpy(character_details->uuid, uuid, UUID_SIZE);
	character_details->name = strdup("Test Entity");
	character_details->description = strdup("A proud Test Entity");

	register_character_details(game->components, character_details);
}


/**
 * Generate and return a 36 character uuid.
 *
 * Takes the following parameters:
 *   destination - a pointer to a character array to store the uuid
 *   size - the size of the destination buffer, used to verify uuid will fit
 *
 * Returns the generated uuid.
**/
void generate_entity_uuid(char * destination, size_t size) {
	assert(destination);

	if (size < UUID_SIZE) {
		zlog_error(gc, "Error when generating uuid, destination buffer supplied was not big enough.");

		return;
	}

	uuid_t uuid_bin;
	uuid_generate_random(uuid_bin);
	uuid_unparse_lower(uuid_bin, destination);
}