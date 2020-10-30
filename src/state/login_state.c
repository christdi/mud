#include <assert.h>
#include <string.h>


#include "mud/state/login_state.h"
#include "mud/state/play_state.h"
#include "mud/player.h"


void get_character_name(player_t * player, game_t * game, char * input);
void get_new_character_name(player_t * player, game_t * game, char * input);
void get_new_character_password(player_t * player, game_t * game, char * input);


/**
 * Method called when players are in the login state.
**/
void login_state(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	send_to_player(player, "Welcome.  Enter your character name or type 'new' to create one: ");

	player->state = get_character_name;
}


/**
 * Module private method to retrieve the character name the player would like to play.
**/
void get_character_name(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (strncmp(input, "new", COMMAND_SIZE) == 0) {
		send_to_player(player, "Which character name would you like to go by? ");

		player->state = get_new_character_name;

		return;
	}

	// TODO: Existing character
}


/**
 * Module private method to retrieve a new character name.
**/
void get_new_character_name(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (strnlen(input, COMMAND_SIZE) > USERNAME_SIZE) {
		send_to_player(player, "Your name must be at most %d characters long, try again: ", USERNAME_SIZE);

		return;
	}

	strncpy(player->username, input, USERNAME_SIZE);

	send_to_player(player, "Okay [bcyan]%s[reset], what password would you like to use? ", input);

	player->state = get_new_character_password;
}


/**
 * Module private method to retrieve a new character password
**/
void get_new_character_password(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (strnlen(input, COMMAND_SIZE) > PASSWORD_SIZE) {
		send_to_player(player, "Your password must be at most %d characters long, try again: ", USERNAME_SIZE);

		return;
	}

	// TODO: Hash password
	strncpy(player->password_hash, input, PASSWORD_SIZE);

	send_to_player(player, "Very well.  Enjoy yourself!\n\r");

	player->state = play_state;
	player->state(player, game, NULL);
}