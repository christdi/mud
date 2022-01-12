#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "bsd/string.h"

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/db/db.h"
#include "mud/ecs/entity.h"
#include "mud/lua/hooks.h"
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

  if (entity->name != NULL) {
    free(entity->name);
  }

  if (entity->description != NULL) {
    free(entity->description);
  }

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

  LOG(INFO, "Loading entities");

  linked_list_t* entities = create_linked_list_t();

  if (db_entity_load_all(game->database, entities) == -1) {
    LOG(ERROR, "Entities could not be retrieved from the database");

    free_linked_list_t(entities);

    return -1;
  };

  if (lua_hook_on_entities_loaded(game->lua_state, entities) != 0) {
    LOG(ERROR, "Lua on entities loaded hook could not be called");

    free_linked_list_t(entities);

    return -1;
  }

  it_t it = list_begin(entities);

  entity_t* entity = NULL;

  while ((entity = (entity_t*)it_get(it)) != NULL) {
    hash_table_insert(game->entities, entity->id.raw, entity);

    it = it_next(it);
  }

  free_linked_list_t(entities);

  return 0;
}

/**
 * Searches the game for an entity matching a given uuid.
 *
 * Returns a pointer to the entity if found or NULL if not.
**/
entity_t* get_entity(game_t* game, const char* uuid) {
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
entity_t* new_entity(game_t* game, const char* name, const char* description) {
  entity_t* entity = create_entity_t();
  entity->id = new_uuid();
  entity->name = strdup(name);
  entity->description = strdup(description);
  hash_table_insert(game->entities, entity->id.raw, entity);

  LOG(INFO, "New entity created [%s], [%s]", name, description);

  return entity;
}
