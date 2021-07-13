#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/command/admin.h"
#include "mud/command/command.h"
#include "mud/command/communication.h"
#include "mud/command/explore.h"
#include "mud/command/general.h"
#include "mud/dbo/command_dbo.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/game.h"
#include "mud/log.h"

const command_t* command_lookup(const char* name);

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
 * Populates the commands hash table from a static array of commands.
**/
const command_t* command_lookup(const char* name) {
  static const command_t commands[] = {
    { "function_entity", entity_command },
    { "function_shutdown", shutdown_command },
    { "function_inventory", inventory_command },
    { "function_look", look_command },
    { "function_quit", quit_command },
    { "function_say", say_command },
    { "\0", NULL }
  };

  const command_t* command = commands;

  while (command->func != NULL) {
    if (strncmp(command->name, name, COMMAND_NAME_MAX_LENGTH-1) == 0) {
      return command;
    }

    command++;
  }

  return NULL;
}

/**
 * Attempts to retrieve a command for execution.
 *
 * Parameters
 *  game - game_t structure storing database and commands
 *  name - the name of the command to be searched for
 *
 * Returns a pointer to a valid command_t structure on success or NULL on failure.
**/
const command_t * get_command(game_t* game, const char *name) {
  linked_list_t* commands = create_linked_list_t();
  commands->deallocator = deallocate_command_dbo_t;

  int count = 0;

  if ((count = select_commands_by_name(game, name, commands)) <= 0) {
    if (count == -1) {
      zlog_error(gc, "get_command(): Unable to retreive commands from database matching [%s]", name);
    }

    free_linked_list_t(commands);

    return NULL;
  }

  /* TODO(Chris I): Don't just select the first command.  Filter for appropriate command */
  command_dbo_t* command_dbo = NULL;

  list_at(commands, 0, (void*)&command_dbo);

  if (command_dbo == NULL) {
    zlog_error(gc, "get_command(): Unable to retreive first command from linked list");

    free_linked_list_t(commands);

    return NULL;
  }

  const command_t* cmd = command_lookup(command_dbo->function);

  free_linked_list_t(commands);

  return cmd;
}
