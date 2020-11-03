#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "mud/ecs/entity.h"
#include "mud/action/action_callback.h"
#include "mud/ecs/character_details.h"
#include "mud/data/hash_table.h"
#include "mud/util/muduuid.h"
#include "mud/log.h"
#include "mud/player.h"
#include "mud/game.h"


/**
 * Allocates and initialises a new entity_t struct.
 *
 * Returns a pointer to the newly allocated entity_t struct.
**/
entity_t * create_entity_t() {
	entity_t * entity = calloc(1, sizeof * entity);
	entity->action_callback = create_action_callback_t();

	return entity;
}


/**
 * Frees an allocated entity_t struct.
**/
void free_entity_t(entity_t * entity) {
	assert(entity);

	free_action_callback_t(entity->action_callback);

	free(entity);
}

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

	entity_t * entity = create_entity_t();
	generate_uuid(entity->uuid, UUID_SIZE);
	hash_table_insert(game->entities, entity->uuid, entity);

	character_details_t * character_details = create_character_details_t();
	strncpy(character_details->uuid, entity->uuid, UUID_SIZE);
	character_details->name = strdup("Test Entity");
	character_details->description = strdup("A proud Test Entity");

	register_character_details(game->components, character_details);

	entity = create_entity_t();
	generate_uuid(entity->uuid, UUID_SIZE);
	hash_table_insert(game->entities, entity->uuid, entity);
}


/**
 * Attempts to look up the entity associated with a given player.
**/
entity_t * get_player_entity(game_t * game, player_t * player) {
	return NULL;
}

/**
 * Searches the game for an entity matching a given uuid.
 *
 * Returns a pointer to the entity if found or NULL if not.
**/
entity_t * get_entity(game_t * game, char * uuid) {
	assert(game);
	assert(uuid);

	return (entity_t *) hash_table_get(game->entities, uuid);
}

/**
 * Assigns an entity to a given player.
**/
void assign_entity(entity_t * entity, player_t * player) {
	assert(player);
	assert(entity);

	player->entity = entity;
	entity->player = player;
}
