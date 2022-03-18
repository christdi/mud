#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/data/linked_list.h"
#include "mud/ecs/system.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/game.h"

/**
 * Creates a new instance of system_t.
 *
 * name - Name of this system
 * ref - Lua reference to system table
 *
 * Returns new instance of system_t
**/
system_t* ecs_new_system_t(const char* name, int ref) {
  system_t* system = calloc(1, sizeof(system_t));

  system->uuid = new_uuid();
  system->name = strdup(name);
  system->enabled = true;
  system->ref = ref;

  return system;
}

/**
 * Frees an allocated instance of system_t
 *
 * system - the system to free
**/
void ecs_free_system_t(system_t* system) {
  assert(system);

  if (system->name != NULL) {
    free(system->name);
  }

  free(system);
}

/**
 * Frees a void pointer to an instance of system_t
 *
 * value - the void pointer to system_t
**/
void ecs_deallocate_system_t(void* value) {
  assert(value);

  ecs_free_system_t(value);
}

/**
 * Enables a system.
 *
 * system - the system to enable
**/
void ecs_enable_system(system_t* system) {
  system->enabled = true;
}

/**
 * Disables a system
 *
 * system - the system to disable
**/
void ecs_disable_system(system_t* system) {
  system->enabled = false;
}

/**
 * Runs systems registered with the game.
 *
 * game - game instance containing systems
 **/
void ecs_update_systems(game_t* game) {
  it_t it = list_begin(game->systems);

  system_t* system = NULL;

  while ((system = it_get(it)) != NULL) {
    if (system->enabled) {
      if (lua_hook_on_system_execute(game->lua_state, system) == -1) {
        LOG(ERROR, "Failed to execute system");
      }
    }

    it = it_next(it);
  }
}
