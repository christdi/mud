#include "mud/player.h"
#include "mud/log/log.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void get_player_username(player_t * player, char * username);

/**
 * Allocates and initialises a new player_t struct.
 *
 * Returns the allocated player_t struct.
**/
player_t * create_player_t() {
	player_t * player = calloc(1, sizeof * player);

	return player;
}


/**
 * Frees a player_t struct.
**/
void free_player_t(player_t * player) {
	free(player);

	player = NULL;
}


/**
 * Attempts to send formatted outputted to a player.  Will check if the underlying
 * client_t is valid before attempting to write.
 *
 * Returns 0 on success or -1 on failure
**/
int send_to_player(player_t * player, const char * fmt, ...) {
	assert(player);
	assert(fmt);
	
	if (player->client == NULL) {
		char username[MAX_USERNAME_SIZE];
		get_player_username(player, username);

		zlog_warn(gc, "Send to player with username [%s] failed as they have no client", username);

		return -1;
	}

	char output[MAX_SEND_SIZE];
	
	va_list args;
	va_start(args, fmt);
	vsprintf(output, fmt, args);
	va_end(args);

	return send_to_client(player->client, output);
}


/**
 * Copies the players username into the buffer pointed to by username. If
 * the player does not have a username yet, it is replaced with a placeholder.
**/
void get_player_username(player_t * player, char * username) {
	assert(player);
	assert(username);

	strncpy(username, player->username ? player->username : "anonymous", MAX_USERNAME_SIZE);
}