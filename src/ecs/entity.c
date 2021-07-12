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
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
**/
void deallocate_entity(void* value) {
  assert(value);
  
  entity_t* entity = (entity_t*)value;

  free_entity_t(entity);
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
  // entity_t* character = new_character(game, "Test character", "A proud test character.");
  // entity_t* item = new_item(game, "Excalibur", "A sword that grants ultimate authority.");

  // location_t* character_location = get_location(game->components, character);
  // character_location->at = location->id;

  // location_t* item_location = get_location(game->components, item);
  // item_location->at = location->id;
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
  entity_t* entity = create_entity_t();
  entity->id = entity_id();
  hash_table_insert(game->entities, entity->id.uuid, entity);

  description_t* description_component = create_description_t();
  description_component->entity_id = entity->id;
  description_component->name = strdup(name);
  description_component->description = strdup(description);
  register_description(game->components, description_component);

  location_t* location_component = create_location_t();
  location_component->entity_id = entity->id;
  register_location(game->components, location_component);

  return entity;
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
  entity_t* entity = create_entity_t();
  entity->id = entity_id();
  hash_table_insert(game->entities, entity->id.uuid, entity);

  description_t* description_component = create_description_t();
  description_component->entity_id = entity->id;
  description_component->name = strdup(name);
  description_component->description = strdup(description);
  register_description(game->components, description_component);

  location_t* location_component = create_location_t();
  location_component->entity_id = entity->id;
  register_location(game->components, location_component);

  return entity;
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
  entity_t* entity = create_entity_t();
  entity->id = entity_id();
  hash_table_insert(game->entities, entity->id.uuid, entity);

  description_t* description_component = create_description_t();
  description_component->entity_id = entity->id;
  description_component->name = strdup(name);
  description_component->description = strdup(description);
  register_description(game->components, description_component);

  location_t* location_component = create_location_t();
  location_component->entity_id = entity->id;
  register_location(game->components, location_component);

  return entity;
}
