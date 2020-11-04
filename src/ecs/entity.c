#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "mud/ecs/entity.h"
#include "mud/action/action_callback.h"
#include "mud/ecs/description.h"
#include "mud/ecs/container.h"
#include "mud/ecs/contained.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
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

	// Create room
	entity_t * room = create_entity_t();
	generate_uuid(room->uuid, UUID_SIZE);
	hash_table_insert(game->entities, room->uuid, room);

	description_t * room_description = create_description_t();
	strncpy(room_description->uuid, room->uuid, UUID_SIZE);
	room_description->name = strdup("Test room");
	room_description->description = strdup("A bare test room.");
	register_description(game->components, room_description);

	container_t * room_container = create_container_t();
	strncpy(room_container->uuid, room->uuid, UUID_SIZE);
	register_container(game->components, room_container);


	// Create character
	entity_t * character = create_entity_t();
	generate_uuid(character->uuid, UUID_SIZE);
	hash_table_insert(game->entities, character->uuid, character);

	description_t * character_description = create_description_t();
	strncpy(character_description->uuid, character->uuid, UUID_SIZE);
	character_description->name = strdup("Test character");
	character_description->description = strdup("A proud test character.");
	register_description(game->components, character_description);

	contained_t * character_contained = create_contained_t();
	strncpy(character_contained->uuid, character->uuid, UUID_SIZE);
	register_contained(game->components, character_contained);

	container_t * character_container = create_container_t();
	strncpy(character_container->uuid, character->uuid, UUID_SIZE);
	register_container(game->components, character_container);


	// Create items
	entity_t * item = create_entity_t();
	generate_uuid(item->uuid, UUID_SIZE);
	hash_table_insert(game->entities, item->uuid, item);

	description_t * item_description = create_description_t();
	strncpy(item_description->uuid, item->uuid, UUID_SIZE);
	item_description->name = strdup("Test item");
	item_description->description = strdup("A useful test item.");
	register_description(game->components, item_description);

	contained_t * item_contained = create_contained_t();
	strncpy(item_contained->uuid, item->uuid, UUID_SIZE);
	register_contained(game->components, item_contained);

	// Link containers
	strncpy(character_contained->container, room->uuid, UUID_SIZE);
	strncpy(item_contained->container, character->uuid, UUID_SIZE);
	list_add(room_container->contains, character->uuid);
	list_add(character_container->contains, item->uuid);
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
