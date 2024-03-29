#ifndef MUD_ECS_ENTITY_H
#define MUD_ECS_ENTITY_H

#include "mud/util/muduuid.h"

/**
 * Typedefs
 **/
typedef struct game game_t;
typedef struct entity entity_t;

/**
 * Structs
 **/
typedef struct entity {
  mud_uuid_t id;
} entity_t;

/**
 * Function prototypes
 **/
entity_t* ecs_new_entity_t();
void ecs_free_entity_t(entity_t* entity);
void ecs_deallocate_entity(void* value);

int ecs_load_entities(game_t* game);

entity_t* ecs_get_entity(game_t* game, const char* uuid);
entity_t* ecs_new_entity(game_t* game);
int ecs_save_entity(game_t* game, entity_t* entity);
int ecs_delete_entity(game_t* game, entity_t* entity);

#endif