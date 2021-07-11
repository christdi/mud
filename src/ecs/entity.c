#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "bsd/string.h"

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/component/description.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/component/location.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/player.h"
#include "mud/util/muduuid.h"

/**
 * Allocates and initialises a new entity_t struct.
 *
 * Returns a pointer to the newly allocated entity_t struct.
**/
entity_t* create_entity_t() {
  entity_t* entity = calloc(1, sizeof *entity);

  return entity;
}

/**
 * Frees an allocated entity_t struct.
**/
void free_entity_t(entity_t* entity) {
  assert(entity);

  free(entity);
}

/**
 * Loads entities from persistence into the game.
 *
 * Takes the following parameters:
 *   game - the game_t struct where the entities should be loaded to
**/
void load_entities(game_t* game) {
  assert(game);

  zlog_info(gc, "Loading entities");

  // TODO(Chris I): Actually load entities

  entity_t* location = new_location(game, "Relaxing woodlands", "A beautiful expanse of woodland.");
  entity_t* character = new_character(game, "Test character", "A proud test character.");
  entity_t* item = new_item(game, "Excalibur", "A sword that grants ultimate authority.");

  location_t* character_location = get_location(game->components, character);
  strlcpy(character_location->location_uuid, location->uuid, UUID_SIZE);

  location_t* item_location = get_location(game->components, item);
  strlcpy(item_location->location_uuid, location->uuid, UUID_SIZE);
}

/**
 * Searches the game for an entity matching a given uuid.
 *
 * Returns a pointer to the entity if found or NULL if not.
**/
entity_t* get_entity(game_t* game, char* uuid) {
  assert(game);
  assert(uuid);

  return (entity_t*)hash_table_get(game->entities, uuid);
}

/**
 * Creates and registers the components necessary to represent a character.
 *
 * This function takes the following parameters:
 *   game - a pointer to a game struct containing components
 *   name - the name to use for the new character
 *   description - the description to use for the new character
 *
 * Returns a pointer to an entity struct representing the new character
**/
entity_t* new_character(game_t* game, char* name, char* description) {
  entity_t* character = create_entity_t();
  generate_uuid(character->uuid, UUID_SIZE);
  hash_table_insert(game->entities, character->uuid, character);

  description_t* character_description = create_description_t();
  strncpy(character_description->uuid, character->uuid, UUID_SIZE);
  character_description->name = strdup(name);
  character_description->description = strdup(description);
  register_description(game->components, character_description);

  location_t* location = create_location_t();
  strncpy(location->uuid, character->uuid, UUID_SIZE);
  register_location(game->components, location);

  return character;
}

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
entity_t* new_location(game_t* game, char* name, char* description) {
  entity_t* location = create_entity_t();
  generate_uuid(location->uuid, UUID_SIZE);
  hash_table_insert(game->entities, location->uuid, location);

  description_t* location_description = create_description_t();
  strncpy(location_description->uuid, location->uuid, UUID_SIZE);
  location_description->name = strdup(name);
  location_description->description = strdup(description);
  register_description(game->components, location_description);

  location_t* parent_location = create_location_t();
  strncpy(parent_location->uuid, location->uuid, UUID_SIZE);
  register_location(game->components, parent_location);

  return location;
}
