#include <assert.h>


#include "mud/game.h"
#include "mud/player.h"
#include "mud/log.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/description.h"
#include "mud/command/command.h"
#include "mud/util/mudstring.h"
#include "mud/state/play_state.h"


void send_prompt(player_t * player, game_t * game);


/**
 * State method for players who are currently in the game world.
**/
void play_state(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (!input) {
		send_to_all_players(game, NULL, "\n\r[bcyan]%s[reset] has entered the world.\n\r", player->username);

		send_prompt(player, game);

		return;
	}

	char command[COMMAND_SIZE];
	input = extract_argument(input, command);
	command_t * cmd = get_command(game, trim(command));

	if (cmd == NULL) {
		send_to_player(player, "[bcyan]%s[reset] command not recognised.\n\r", command);

		send_prompt(player, game);

		return;
	}

	cmd->func(player, game, trim(input));

	send_prompt(player, game);
}


/**
 * Send a prompt to a player.
**/
void send_prompt(player_t * player, game_t * game) {
	if (player->entity) {
		description_t * description = get_description(game->components, player->entity);

		if (description) {
			send_to_player(player, "[bgreen]%s[reset] => ", description->name);
			return;
		}
	}

	send_to_player(player, "[bgreen]%s[reset] => ", player->username);
}