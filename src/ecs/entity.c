#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "bsd/string.h"

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/dbo/entity_dbo.h"
#include "mud/ecs/component/location.h"
#include "mud/ecs/entity.h"
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
 *
 * Returns 0 on success or -1 on failure
**/
int load_entities(game_t* game) {
  assert(game);

  mlog(INFO, "load_entities", "Loading entities");

  linked_list_t* entities = create_linked_list_t();
  entities->deallocator = entity_dbo_t_deallocate;

  if (entity_dbo_load_all(game, entities) == -1) {
    mlog(ERROR, "load_entities", "Entities could not be retrieved from the database");
    return -1;
  };

  it_t it = list_begin(entities);

  entity_dbo_t* entity_dbo = NULL;

  while ((entity_dbo = (entity_dbo_t*)it_get(it)) != NULL) {
    entity_t* entity = create_entity_t();
    entity_from_entity_dbo(entity, entity_dbo);

    hash_table_insert(game->entities, entity->id.uuid, entity);

    it = it_next(it);
  }

  free_linked_list_t(entities);

  return 0;
}

/**
 * Populates an entity_t with the data from an entity_dbo_t.
 *
 * Parameters
 *  entity - the entity_t to tbe populated
 *  entity_dbo - the entity_dbo to be populated from
**/
void entity_from_entity_dbo(entity_t* entity, entity_dbo_t* entity_dbo) {
  if (entity_dbo->uuid != NULL) {
    strlcpy(entity->id.uuid, entity_dbo->uuid, UUID_SIZE);
  }

  if (entity_dbo->name != NULL) {
    entity->name = strdup(entity_dbo->name);
  }

  if (entity_dbo->description != NULL) {
    entity->description = strdup(entity_dbo->description);
  }
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
 * Creates and registers a new entity.
 *
 * This function takes the following parameters:
 *   game - a pointer to a game struct containing components
 *   name - the name to use for the new entity
 *   description - the description to use for the new entity
 *
 * Returns a pointer to an entity struct representing the new entity
**/
entity_t* new_entity(game_t* game, char* name, char* description) {
  entity_t* entity = create_entity_t();
  entity->id = entity_id();
  entity->name = strdup(name);
  entity->description = strdup(description);
  hash_table_insert(game->entities, entity->id.uuid, entity);

  return entity;
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
  entity->name = strdup(name);
  entity->description = strdup(description);
  hash_table_insert(game->entities, entity->id.uuid, entity);

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
  entity->name = strdup(name);
  entity->description = strdup(description);
  hash_table_insert(game->entities, entity->id.uuid, entity);

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
  entity->name = strdup(name);
  entity->description = strdup(description);
  hash_table_insert(game->entities, entity->id.uuid, entity);

  location_t* location_component = create_location_t();
  location_component->entity_id = entity->id;
  register_location(game->components, location_component);

  return entity;
}
