#include <assert.h>
#include <string.h>


#include "mud/state/login_state.h"
#include "mud/state/play_state.h"
#include "mud/dbo/account.h"
#include "mud/util/mudhash.h"
#include "mud/player.h"


void get_account_name(player_t * player, game_t * game, char * input);
void get_account_password(player_t * player, game_t * game, char * input);
void get_new_account_name(player_t * player, game_t * game, char * input);
void get_new_account_password(player_t * player, game_t * game, char * input);
void validate_new_account_password(player_t * player, game_t * game, char * input);


void login_state(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	player_change_state(player, game, get_account_name);
}


void get_account_name(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (input == NULL) {
		send_to_player(player, "Enter your [bgreen]username[reset] or type [bgreen]new[reset] to create one: ");
		return;
	}

	if (strncmp(input, "new", COMMAND_SIZE) == 0) {
		player_change_state(player, game, get_new_account_name);
		return;
	}

	strncpy(player->account->username, input, USERNAME_SIZE);
	player_change_state(player, game, get_account_password);
}


void get_account_password(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (input == NULL) {
		send_to_player(player, "What is the [bgreen]password[reset] for this account? ");
		return;
	}

	char password_hash[SHA256_SIZE];
	string_to_sha256(input, password_hash);

	if (account_validate(game, player->account->username, password_hash) == -1) {
		send_to_player(player, "No match for that [bgreen]username[reset] and [bgreen]password[reset] combination.\n\r");
		player_change_state(player, game, get_account_name);
		return;
	}

	player_change_state(player, game, play_state);
}


void get_new_account_name(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (input == NULL) {
		send_to_player(player, "What [bgreen]username[reset] would you like to use? ");
		return;
	}

	if (strnlen(input, COMMAND_SIZE) > USERNAME_SIZE) {
		send_to_player(player, "Your [bgreen]username[reset] must be at most [bred]%d[reset] characters long, try again: ", USERNAME_SIZE);
		return;
	}

	if (account_exists(game, input) == 0) {
		send_to_player(player, "[bgreen]%s[reset] is already in use.  Please enter another: ", input);
		return;
	}

	strncpy(player->account->username, input, USERNAME_SIZE);
	player_change_state(player, game, get_new_account_password);
}


void get_new_account_password(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (input == NULL) {
		send_to_player(player, "Okay [bgreen]%s[reset], what [bgreen]password[reset] would you like to use? ", player->account->username);
		return;
	}

	if (strnlen(input, COMMAND_SIZE) > PASSWORD_SIZE) {
		send_to_player(player, "Your [bgreen]password[reset] must be at most [bred]%d[reset] characters long, try again: ", PASSWORD_SIZE);
    return;
	}

	string_to_sha256(input, player->account->password_hash);
	player_change_state(player, game, validate_new_account_password);
}


void validate_new_account_password(player_t * player, game_t * game, char * input) {
	assert(player);
	assert(game);

	if (input == NULL) {
		send_to_player(player, "Please re-enter your [bgreen]password[reset]: ");
		return;
	}

	char password_hash[SHA256_SIZE];
	string_to_sha256(input, password_hash);

	if (strncmp(player->account->password_hash, password_hash, SHA256_DIGEST_LENGTH * 2) != 0) {
		send_to_player(player, "Sorry, [bgreen]password[reset] did not match.  You will be asked for a new [bgreen]password[reset].\n\r");
		player_change_state(player, game, get_new_account_password);
		return;
	}

	account_save(game, player->account);
	player_change_state(player, game, play_state);
}