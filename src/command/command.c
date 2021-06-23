#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "mud/game.h"
#include "mud/log.h"
#include "mud/command/admin.h"
#include "mud/command/command.h"
#include "mud/command/communication.h"
#include "mud/command/explore.h"
#include "mud/command/general.h"
#include "mud/data/hash_table/hash_table.h"


/**
 * Allocate memory for and initialize a command_t.
 *
 * Returns the newly allocated command_t
**/
command_t * create_command_t() {
	command_t * command = calloc(1, sizeof * command);

	return command;
}


/**
 * Frees an allocated command_t.
**/
void free_command_t(command_t * command) {
	free(command);
}

/**
 * Populates the commands hash table from a static array of commands.
**/
void load_commands(game_t * game) {
	static command_t commands[] = {
		{ "entity", entity_command },
		{ "inventory", inventory_command },
		{ "i", inventory_command },
		{ "look", look_command },
		{ "l", look_command },
		{ "quit", quit_command },
		{ "say", say_command },
		{ "\0", NULL  }
	};

	zlog_info(gc, "Loading commands");

	command_t * command;

	for (command = commands; command->func != NULL; command++) {
		hash_table_insert(game->commands, command->name, command);
	}
}


/**
 * Attempts to find a command matching a given name.
 *
 * Returns a pointer to the command_t or NULL if not found.
**/
command_t * get_command(game_t * game, char * name) {
	command_t * command = (command_t *) hash_table_get(game->commands, name);

	return command;
}