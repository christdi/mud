#include <assert.h>


#include "mud/game.h"
#include "mud/player.h"
#include "mud/command/command.h"
#include "mud/log/log.h"
#include "mud/util/mudstring.h"
#include "mud/state/play_state.h"


void send_prompt(player_t * player);


/**
 * State method for players who are currently in the game world.
**/
void play_state(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (!input) {
		send_to_all_players(game, NULL, "\n\r[bcyan]%s[reset] has entered the world.\n\r", player->username);

		send_prompt(player);

		return;
	}

	char command[COMMAND_SIZE];
	input = extract_argument(input, command);
	command_t * cmd = get_command(game, trim(command));

	if (cmd) {
		cmd->func(player, game, trim(input));
	} else {
		send_to_player(player, "[bcyan]%s[reset] command not recognised.\n\r", command);
	}
	

	send_prompt(player);
}


/**
 * Send a prompt to a player.
**/
void send_prompt(player_t * player) {
	send_to_player(player, "[bgreen]%s[reset] => ", player->username);
}