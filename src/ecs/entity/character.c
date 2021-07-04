#include <string.h>

#include "mud/data/hash_table.h"
#include "mud/ecs/description.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/entity/character.h"
#include "mud/ecs/location.h"
#include "mud/game.h"
#include "mud/util/muduuid.h"

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