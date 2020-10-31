#include "mud/command/communication.h"
#include "mud/log/log.h"
#include "mud/util/mudstring.h"
#include "mud/player.h"


/**
 * Command which broadcasts a statement to all players connected to the game.
**/
void say_command(player_t * player, game_t * game, char * input) {
	if (!input || *input == '\0') {
		send_to_player(player, "Say what?\n\r");

		return;
	}

	// TODO: Should only communicate to those in the same room
	send_to_player(player, "\n\rYou say '[bwhite]%s[reset]'.\n\r", input);
	send_to_all_players(game, player, "\n\r[bcyan]%s[reset] says '[bwhite]%s[reset]'.\n\r", player->username, input);	
}