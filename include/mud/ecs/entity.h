#ifndef MUD_ECS_ENTITY_H
#define MUD_ECS_ENTITY_H

#include <stdlib.h>

#include "mud/ecs/entity_id.h"

/**
 * Typedefs
**/
typedef struct game game_t; /* game.h */
typedef struct entity_dbo entity_dbo_t; /* entity_dbo.h */

/**
 * Structs
**/
typedef struct entity {
  entity_id_t id;
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
entity_t* get_entity(game_t* game, char* uuid);

void entity_from_entity_dbo(entity_t* entity, entity_dbo_t* entity_dbo);

entity_t* new_entity(game_t* game, char* name, char* description);

#endif