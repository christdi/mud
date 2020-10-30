#include "mud/game.h"
#include "mud/player.h"
#include "mud/data/hash_table/hash_table.h"
#include "mud/data/hash_table/hash_iterator.h"
#include "mud/log/log.h"
#include "mud/state/login_state.h"


#include <assert.h>
#include <stdlib.h>
#include <string.h>


void get_player_username(player_t * player, char * username);
void write_to_player(player_t * player, char * output);

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
 * Callback from the network module when a new client connects.
**/
void player_connected(client_t * client, void * context) {
  game_t * game = (game_t *) context;

  player_t * player = create_player_t();
  player->client = client;
  player->state = login_state;

  hash_table_insert(game->players, client->uuid, player);

  player->state(player, game, NULL);
}


/**
 * Callback from the network module when a client disconnects.
**/
void player_disconnected(client_t * client, void * context) {
  game_t * game = (game_t *) context;

  player_t * player = hash_table_delete(game->players, client->uuid);

  free_player_t(player);
}


/**
 * Callback from the network module when a client receives input.
**/
void player_input(client_t * client, void * context) {
  game_t * game = (game_t *) context;

  player_t * player = hash_table_get(game->players, client->uuid);

  char command[COMMAND_SIZE];

  if (extract_from_input(client, command, COMMAND_SIZE, "\r\n") != -1 ) {
    if (strnlen(command, COMMAND_SIZE) > 0) {
      player->state(player, game, command);
    }
  }
}


/**
 * Attempts to send formatted outputted to a player.  Will check if the underlying
 * client_t is valid before attempting to write.
 *
 * Returns 0 on success or -1 on failure
**/
void send_to_player(player_t * player, const char * fmt, ...) {
	assert(player);
	assert(fmt);

	char output[SEND_SIZE];
	
	va_list args;
	va_start(args, fmt);
	vsprintf(output, fmt, args);
	va_end(args);

	write_to_player(player, output);
}


/**
 * Sends a formatted message to all connected players.  May optionally exclude a player
 * by specifying them in the excluding parameter.
**/
void send_to_all_players(game_t * game, player_t * excluding, const char * fmt, ...) {
	assert(game);
  assert(game->players);

	char output[SEND_SIZE];

  h_it_t it = hash_table_iterator(game->players);
  player_t * target;

	va_list args;
	va_start(args, fmt);
	vsprintf(output, fmt, args);

  while ((target = h_it_get(it)) != NULL) {
  	if (excluding && excluding == target) {
  		it = h_it_next(it);
  		
  		continue;
  	}

      write_to_player(target, output);

      it = h_it_next(it);
    }

    va_end(args);
}


/**
 * Writes a character array to a player.  Ensures that they first have a client.
**/
void write_to_player(player_t * player, char * output) {
	assert(player);
	assert(output);

	if (player->client == NULL) {
		char username[USERNAME_SIZE];
		get_player_username(player, username);

		zlog_warn(gc, "Send to player with username [%s] failed as they have no client", username);

    return;
	}

	if (send_to_client(player->client, output) != 0) {
		zlog_warn(gc, "Send to player failed, unable to write to client [%s]", player->client->uuid);

    return;
	}
}


/**
 * Copies the players username into the buffer pointed to by username. If
 * the player does not have a username yet, it is replaced with a placeholder.
**/
void get_player_username(player_t * player, char * username) {
	assert(player);
	assert(username);

	strncpy(username, player->username[0] != '\0' ? player->username : "anonymous", USERNAME_SIZE);
}