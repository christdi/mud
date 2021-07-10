#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "bsd/string.h"

#include "mud/action/action_callback.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/description.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/entity/character.h"
#include "mud/ecs/entity/item.h"
#include "mud/ecs/entity/location.h"
#include "mud/ecs/location.h"
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
  entity->action_callback = create_action_callback_t();

  return entity;
}

/**
 * Frees an allocated entity_t struct.
**/
void free_entity_t(entity_t* entity) {
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
 * Assigns an entity to a given player.
**/
void assign_entity(entity_t* entity, player_t* player) {
  assert(player);
  assert(entity);

  player->entity = entity;
  entity->player = player;
}
