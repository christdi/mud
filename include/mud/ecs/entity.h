#ifndef MUD_ECS_ENTITY_H
#define MUD_ECS_ENTITY_H

#include "mud/util/muduuid.h"

/**
 * Typedefs
**/
typedef struct game game_t; /* game.h */

/**
 * Structs
**/
typedef struct entity {
  mud_uuid_t id;
  char* name;
  char* description;
} entity_t;

/**
 * Function prototypes
**/
entity_t* create_entity_t();
void free_entity_t(entity_t* entity);
void deallocate_entity(void* value);

int load_entities(game_t* game);
entity_t* get_entity(game_t* game, const char* uuid);

entity_t* new_entity(game_t* game, const char* name, const char* description);

#endif