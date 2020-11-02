#include <assert.h>

#include "mud/command/communication.h"
#include "mud/ecs/character_details.h"
#include "mud/log/log.h"
#include "mud/util/mudstring.h"
#include "mud/game.h"
#include "mud/player.h"

/**
 * Command which broadcasts a statement to all players connected to the game.
**/
void say_command(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);
	assert(input);

	if (!has_character_details(game->components, player->entity)) {
		send_to_player(player, "\n\rYou're not capable of doing that.\n\r");

		return;
	}

	if (*input == '\0') {
		send_to_player(player, "Say what?\n\r");

		return;
	}

	character_details_t * character_details = get_character_details(game->components, player->entity);

	// TODO: Should only communicate to those in the same room
	send_to_player(player, "\n\rYou say '[bwhite]%s[reset]'.\n\r", input);
	send_to_all_players(game, player, "\n\r[bcyan]%s[reset] says '[bwhite]%s[reset]'.\n\r", character_details->name, input);
}