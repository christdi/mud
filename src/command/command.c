#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/command/admin.h"
#include "mud/command/command.h"
#include "mud/command/command_function.h"
#include "mud/command/communication.h"
#include "mud/command/explore.h"
#include "mud/command/general.h"
#include "mud/data/hash_table/hash_table.h"
#include "mud/game.h"
#include "mud/log.h"

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
void load_commands(game_t* game) {
  static command_function_t commands[] = {
    { "function_entity", entity_command },
    { "function_shutdown", shutdown_command },
    { "function_inventory", inventory_command },
    { "function_look", look_command },
    { "function_quit", quit_command },
    { "function_say", say_command },
    { "\0", NULL }
  };

  zlog_info(gc, "Loading commands");

  command_function_t* command = NULL;

  for (command = commands; command->func != NULL; command++) {
    hash_table_insert(game->commands, command->name, command);
  }
}

/**
 * Attempts to find a command matching a given name.
 *
 * Returns a pointer to the command_t or NULL if not found.
**/
command_t* get_command(game_t* game, char* name) {
  command_t* command = (command_t*)hash_table_get(game->commands, name);

  return command;
}
