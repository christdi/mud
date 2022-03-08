#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/command/command.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/db/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/script.h"
#include "mud/player.h"

static int get_command(game_t* game, const char* name, command_t* command);

/**
 * Allocate memory for and initialize a command_t.
 *
 * Returns the newly allocated command_t
**/
command_t* create_command_t() {
  command_t* command = calloc(1, sizeof *command);

  return command;
}

/**
 * Frees an allocated command_t.
**/
void free_command_t(command_t* command) {
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
void deallocate_command(void* value) {
  assert(value);

  command_t* command = (command_t*)value;

  free_command_t(command);
}

/**
 * TODO(Chris I)
**/
int execute_command(game_t* game, player_t* player, const char* command, const char* arguments) {
  command_t* cmd = create_command_t();

  if (get_command(game, command, cmd) == -1) {
    free_command_t(cmd);

    return -1;
  }

  const char* script_uuid = uuid_str(&cmd->script);

  if (script_run_command_script(game, script_uuid, player, arguments) == -1) {
    LOG(ERROR, "Failed to load script with uuid [%s]", script_uuid);

    free_command_t(cmd);

    return -1;
  }

  free_command_t(cmd);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int get_command(game_t* game, const char* name, command_t* command) {
  linked_list_t* commands = create_linked_list_t();
  commands->deallocator = deallocate_command;

  int count = 0;

  if ((count = db_command_find_by_name(game->database, name, commands)) <= 0) {
    if (count == -1) {
      LOG(ERROR, "Unable to retreive commands from database matching [%s]", name);
    }

    free_linked_list_t(commands);

    return -1;
  }

  /* TODO(Chris I): Don't just select the first command.  Filter for appropriate command */
  command_t* list_cmd = NULL;

  list_at(commands, 0, (void*)&list_cmd);

  if (list_cmd == NULL) {
    LOG(ERROR, "Unable to retreive first command from linked list");

    free_linked_list_t(commands);

    return -1;
  }

  command->name = strdup(list_cmd->name);
  command->script = list_cmd->script;

  free_linked_list_t(commands);

  return 0;
}
