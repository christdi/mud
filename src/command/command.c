#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/command/admin.h"
#include "mud/command/command.h"
#include "mud/command/communication.h"
#include "mud/command/explore.h"
#include "mud/command/general.h"
#include "mud/db/db.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/dbo/command_dbo.h"
#include "mud/game.h"
#include "mud/log.h"

const cmd_func_t* command_lookup(const char* name);

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

  if(command->function != NULL) {
    free(command->function);
  }

  free(command);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
**/
void deallocate_command(void* value) {
  assert(value);

  mlog(INFO, "deallocate_command", "Deallocating");

  command_t* command = (command_t*)value;

  free_command_t(command);
}

/**
 * Populates the commands hash table from a static array of commands.
**/
const cmd_func_t* command_lookup(const char* name) {
  static const cmd_func_t funcs[] = {
    { "function_entity", entity_command },
    { "function_shutdown", shutdown_command },
    { "function_inventory", inventory_command },
    { "function_look", look_command },
    { "function_quit", quit_command },
    { "function_say", say_command },
    { "\0", NULL }
  };

  const cmd_func_t* func = funcs;

  while (func->func != NULL) {
    if (strncmp(func->function, name, strlen(func->function)) == 0) {
      return func;
    }

    func++;
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
 * Returns a pointer to an allocated command_t structure on success or NULL on failure.  It is the responsibility
 * of the caller to free the allocated command_t structure after usage. 
 * 
 * TODO(Chris I): This is crap, caller shouldn't have to free the command structure.  Revisit the design of this
 * in the fuutre.
**/
command_t* get_command(game_t* game, const char* name) {
  linked_list_t* commands = create_linked_list_t();
  commands->deallocator = deallocate_command;

  int count = 0;

  if ((count = db_command_find_by_name(game->database, name, commands)) <= 0) {
    mlog(INFO, "get_command", "Found [%d] matching commands", count);

    if (count == -1) {
      mlog(ERROR, "get_command", "Unable to retreive commands from database matching [%s]", name);
    }

    free_linked_list_t(commands);

    return NULL;
  }

  mlog(INFO, "get_command", "Found [%d] matching commands", count);

  /* TODO(Chris I): Don't just select the first command.  Filter for appropriate command */
  command_t* cmd = NULL;

  list_at(commands, 0, (void*)&cmd);

  if (cmd == NULL) {
    mlog(ERROR, "get_command", "Unable to retreive first command from linked list");

    free_linked_list_t(commands);

    return NULL;
  }

  mlog(INFO, "get_command", "Extracted command from linked list");

  const cmd_func_t* func = command_lookup(cmd->function);

  if (func == NULL || func->func == NULL) {
    mlog(ERROR, "get_command", "Unable to retreive command function for command [%s]", name);

    free_linked_list_t(commands);

    return NULL;
  }

  mlog(INFO, "get_command", "Assigning func");

  cmd->func = func->func;

  list_remove(commands, cmd);

  free_linked_list_t(commands);

  return cmd;
}
