#include <assert.h>

#include "mud/state/play_state.h"
#include "mud/game.h"
#include "mud/player.h"


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

	send_to_player(player, "\n\rYou say '[bwhite]%s[reset]'.\n\r", input);
	send_to_all_players(game, player, "\n\r[bcyan]%s[reset] says '[bwhite]%s[reset]'.\n\r", player->username, input);
	send_prompt(player);
}


/**
 * Send a prompt to a player.
**/
void send_prompt(player_t * player) {
	send_to_player(player, "%s => ", player->username);
}