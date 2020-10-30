#include <assert.h>

#include "mud/state/play_state.h"
#include "mud/game.h"
#include "mud/player.h"


void send_prompt(player_t * player);


void play_state(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (!input) {
		send_to_all_players(game, NULL, "\n\r%s has entered the world.\n\r", player->username);
		send_prompt(player);

		return;
	}

	send_to_player(player, "\n\rYou say '%s'.\n\r", input);
	send_to_all_players(game, player, "\n\r%s says '%s'.\n\r", player->username, input);
	send_prompt(player);
}



void send_prompt(player_t * player) {
	send_to_player(player, "%s => ", player->username);
}