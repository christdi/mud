#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdbool.h>

#include "mud/util/muduuid.h"

/**
 * Typedefs
 **/
typedef struct game game_t; /* game.h */
typedef struct lua_ref lua_ref_t;

/**
 * Structs
**/
typedef struct system {
  mud_uuid_t uuid;
  char* name;
  bool enabled;
  lua_ref_t* ref;
} system_t;

/**
 * Function prototypes
 **/
system_t* ecs_new_system_t(const char* name, lua_ref_t* ref);
void ecs_free_system_t(system_t* system);
void ecs_deallocate_system_t(void* value);

void ecs_enable_system(system_t* system);
void ecs_disable_system(system_t* system);

void ecs_update_systems(game_t* game);

#endif