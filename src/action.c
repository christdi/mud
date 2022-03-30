#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/action.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"

/**
 * Allocates a new instance of action_t
 *
 * uuid - uuid of the action
 * name - name of the action
 * script_uuid - uuid of the script this action will run
 *
 * Returns the allocated instance of action_t
 **/
action_t* action_new_action_t(const char* uuid, const char* name, const char* script_uuid) {
  assert(uuid);
  assert(name);
  assert(script_uuid);

  action_t* action = calloc(1, sizeof(action_t));

  action->uuid = str_uuid(uuid);
  action->name = strdup(name);
  action->script = str_uuid(script_uuid);

  return action;
}

/**
 * Frees an allocated instance of action_t
 *
 * action - the action to be freed
 **/
void action_free_action_t(action_t* action) {
  assert(action);

  if (action->name != NULL) {
    free(action->name);
  }

  free(action);
}

/**
 * Deallocated a void pointer to action_t
 *
 * value - void pointer containing action_t
 **/
void action_deallocate_action_t(void* value) {
  assert(value);

  action_free_action_t(value);
}

/**
 * Retrieves all actions from persistence and places them into game instance
 *
 * game - game_t instance containing database and actions
 *
 * Returns 0 on success or -1 on failure
 **/
int action_load_actions(game_t* game) {
  linked_list_t* results = create_linked_list_t();

  int count;

  if ((count = db_action_load_all(game->database, results)) == -1) {
    LOG(ERROR, "Failed to load actions into engine");
    free_linked_list_t(results);

    return -1;
  }

  lua_call_actions_loaded_hook(game->lua_state, results);

  it_t it = list_begin(results);
  action_t* action = NULL;

  while ((action = it_get(it)) != NULL) {
    hash_table_insert(game->actions, uuid_str(&action->uuid), action);

    it = it_next(it);
  }

  LOG(INFO, "Loaded [%d] actions", count);

  free_linked_list_t(results);

  return 0;
}
