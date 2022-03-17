#ifndef MUD_ECS_ACTION_H
#define MUD_ECS_ACTION_H

#include "mud/util/muduuid.h"

/**
 * Typeefs
**/
typedef struct linked_list linked_list_t;
typedef struct game game_t;

/**
 * Structs
**/
typedef struct action {
  mud_uuid_t uuid;
  char* name;
  mud_uuid_t script;
} action_t;

/**
 * Functions
**/
action_t* ecs_new_action_t(const char* uuid, const char* name, const char* script_uuid);
void ecs_free_action_t(action_t* action);
void ecs_deallocate_action_t(void* value);

int ecs_load_actions(game_t* game);

#endif