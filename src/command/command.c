#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "mud/game.h"
#include "mud/command/command.h"
#include "mud/command/communication.h"
#include "mud/data/hash_table/hash_table.h"
#include "mud/log/log.h"


/**
**/
command_t * create_command_t() {
	command_t * command = calloc(1, sizeof * command);

	return command;
}


/**
**/
void free_command_t(command_t * command) {
	free(command);
}

/**
 * 
**/
void load_commands(game_t * game) {
	command_t commands[] = {
		{ "say", say_command },
		{ "\0", NULL  }
	};

	zlog_info(gc, "Loading commands");

	command_t * command;

	for (command = commands; command->func != NULL; command++) {
		command_t * new_command = create_command_t();
		*new_command = *command;

		hash_table_insert(game->commands, new_command->name, new_command);
	}
}


/**
**/
command_t * get_command(game_t * game, char * name) {
	command_t * command = (command_t *) hash_table_get(game->commands, name);

	return command;
}