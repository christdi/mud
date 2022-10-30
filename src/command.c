#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/command.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/data/deallocate.h"
#include "mud/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/lua/script.h"
#include "mud/player.h"

/**
 * Allocate memory for and initialize a command_t.
 *
 * Returns the newly allocated command_t
 **/
command_t* command_new_command_t(const char* uuid, const char* name, const char* script_uuid) {
  command_t* command = calloc(1, sizeof(command_t));

  command->uuid = str_uuid(uuid);
  command->name = strdup(name);
  command->script = str_uuid(script_uuid);

  return command;
}

/**
 * Frees an allocated command_t.
 **/
void command_free_command_t(command_t* command) {
  assert(command);

  if (command->name != NULL) {
    free(command->name);
  }

  free(command);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
 **/
void command_deallocate_command_t(void* value) {
  assert(value);

  command_t* command = (command_t*)value;

  command_free_command_t(command);
}

/**
 * Allocates a new instance of a command_group_t.
 *
 * uuid - The uuid of the command group
 * description - The description of the command group
 *
 * Returns the newly allocated command_group_t
**/
command_group_t* command_new_command_group_t(const char* uuid, const char* description) {
  assert(uuid);
  assert(description);

  command_group_t* command_group = calloc(1, sizeof(command_group_t));

  command_group->uuid = str_uuid(uuid);
  command_group->description = strdup(description);
  command_group->commands = create_linked_list_t();
  command_group->commands->deallocator = deallocate;

  return command_group;
}

/**
 * Frees an allocated command_group_t.
 *
 * group - The command_group_t to free
**/
void command_free_command_group_t(command_group_t* group) {
  assert(group);

  if (group->description != NULL) {
    free(group->description);
  }

  if (group->commands != NULL) {
    free_linked_list_t(group->commands);
  }

  free(group);
}

/**
 * Deallocates a void pointer to a command_group_t.
 *
 * value - The void pointer to the command_group_t to deallocate
**/
void command_deallocate_command_group_t(void* value) {
  assert(value);

  command_free_command_group_t(value);
}

/**
 * Retrieves all commands from persistence and places them into game instance
 *
 * game - game_t instance containing database and commands
 *
 * Returns 0 on success or -1 on failure
 **/
int command_load_commands(game_t* game) {
  linked_list_t* results = create_linked_list_t();

  int count;

  if ((count = db_command_load_all(game->database, results)) == -1) {
    LOG(ERROR, "Failed to load commands into engine");
    free_linked_list_t(results);

    return -1;
  }

  lua_call_commands_loaded_hook(game->lua_state, results);

  it_t it = list_begin(results);
  command_t* command = NULL;

  while ((command = it_get(it)) != NULL) {
    hash_table_insert(game->commands, uuid_str(&command->uuid), command);

    it = it_next(it);
  }

  LOG(INFO, "Loaded [%d] commands", count);

  free_linked_list_t(results);

  return 0;
}

/**
 * Retrieves all command groups from persistence and places them into game instance
 *
 * game - game_t instance containing database and commands groups
 *
 * Returns 0 on success or -1 on failure
 **/
int command_load_command_groups(game_t* game) {
  linked_list_t* results = create_linked_list_t();

  int count;

  if ((count = db_command_group_load_all(game->database, results)) == -1) {
    LOG(ERROR, "Failed to load command groups into engine");
    free_linked_list_t(results);

    return -1;
  }

  lua_call_command_groups_loaded_hook(game->lua_state, results);

  it_t it = list_begin(results);
  command_group_t* group = NULL;

  while ((group = it_get(it)) != NULL) {
    hash_table_insert(game->command_groups, uuid_str(&group->uuid), group);

    it = it_next(it);
  }

  LOG(INFO, "Loaded [%d] command groups", count);

  free_linked_list_t(results);

  return 0;
}

/**
 * Attempts to retrieve a command group by id from a game_t instance
 * 
 * game - the game_t instance to retrieve the command group from
 * 
 * Returns a command group if found or NULL otherwise.
**/
command_group_t* command_get_command_group_by_id(game_t* game, const char* uuid) {
  return hash_table_get(game->command_groups, uuid); 
}
