#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/command.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
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
 * Retrieves all commands from persistence and places them into game instance
 *
 * game - game_t instance containing database and commands
 *
 * Returns 0 on success or -1 on failure
 **/
int command_load_commands(game_t* game) {
  linked_list_t* results = create_linked_list_t();

  int count;

  if ((count = db_command_find_all(game->database, results)) == -1) {
    LOG(ERROR, "Failed to load commands into engine");
    free_linked_list_t(results);

    return -1;
  }

  lua_hook_on_commands_loaded(game->lua_state, results);

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
