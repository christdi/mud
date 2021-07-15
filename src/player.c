#include "mud/player.h"
#include "mud/account.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/state/login_state.h"
#include "mud/state/state.h"
#include "mud/util/mudstring.h"

#include <assert.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_player_username(player_t* player, char* username);
void write_to_player(player_t* player, char* output);

/**
 * Allocates and initialises a new player_t struct.
 *
 * Returns the allocated player_t struct.
**/
player_t* create_player_t() {
  player_t* player = calloc(1, sizeof *player);

  player->account = account_t_new();
  player->state = NULL;
  player->client = NULL;

  return player;
}

/**
 * Frees a player_t struct.
**/
void free_player_t(player_t* player) {
  assert(player);

  if (player->account != NULL) {
    account_t_free(player->account);
  }

  if (player->state != NULL) {
    free_state_t(player->state);
  }

  free(player);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
**/
void deallocate_player(void* value) {
  assert(value);

  player_t* player = (player_t*)value;

  free_player_t(player);
}

/**
 * Callback from the network module when a new client connects.
**/
void player_connected(client_t* client, void* context) {
  game_t* game = (game_t*)context;

  player_t* player = create_player_t();
  player->client = client;

  hash_table_insert(game->players, client->uuid, player);

  player_change_state(player, game, login_state());
}

/**
 * Callback from the network module when a client disconnects.
**/
void player_disconnected(client_t* client, void* context) {
  game_t* game = (game_t*)context;

  hash_table_delete(game->players, client->uuid);
}

/**
 * Callback from the network module when a client receives input.
**/
void player_input(client_t* client, void* context) {
  game_t* game = (game_t*)context;

  player_t* player = hash_table_get(game->players, client->uuid);

  char command[COMMAND_SIZE];

  while (extract_from_input(client, command, COMMAND_SIZE, "\r\n") != -1) {
    if (strnlen(command, COMMAND_SIZE) > 0) {
      if (player->state != NULL && player->state->on_input != NULL) {
        player->state->on_input(player, game, command);
      }
    }
  }
}

/**
 * Method which changes the players current state.  It'll assign the new state
 * and then call it with NULL input which indicates the state is being entered
 * for the first time.
**/
void player_change_state(player_t* player, game_t* game, state_t* state) {
  if (player->state != NULL) {
    if (player->state->on_exit != NULL) {
      player->state->on_exit(player, game);
    }

    free_state_t(player->state);
  }

  player->state = state;

  if (player->state != NULL && player->state->on_enter != NULL) {
    player->state->on_enter(player, game);
  }
}

/**
 * Called on each tick of the game engine, delegates to the state on_tick
 * method if one is defined to allow any time based updates to occur.
 * 
 * Parameters
 *  player - the player who is being ticked
 *  game - game object containing all necessary game data
**/
void player_on_tick(player_t* player, game_t* game) {
  if (player->state != NULL && player->state->on_tick != NULL) {
    player->state->on_tick(player, game);
  }
}

/**
 * Attempts to send formatted outputted to a player.  Will check if the underlying
 * client_t is valid before attempting to write.
 *
 * Returns 0 on success or -1 on failure
**/
void send_to_player(player_t* player, const char* fmt, ...) {
  assert(player);
  assert(fmt);

  char output[SEND_SIZE];

  va_list args;
  va_start(args, fmt);

  if ((vsnprintf(output, SEND_SIZE, fmt, args)) >= SEND_SIZE) {
    mlog(ERROR, "send_to_player", "Formatted output was too long and was truncated");
  }

  va_end(args);

  write_to_player(player, output);
}

void send_to_players(linked_list_t* players, const char* fmt, ...) {
  assert(players);
  assert(fmt);

  char output[SEND_SIZE];

  va_list args;
  va_start(args, fmt);

  if ((vsnprintf(output, SEND_SIZE, fmt, args)) >= SEND_SIZE) {
    mlog(ERROR, "send_to_player", "Formatted output was too long and was truncated");
  }

  va_end(args);

  it_t it = list_begin(players);

  player_t* player = NULL;

  while ((player = (player_t*)it_get(it)) != NULL) {
    write_to_player(player, output);

    it = it_next(it);
  }
}

/**
 * Sends a formatted message to all connected players.  May optionally exclude a player
 * by specifying them in the excluding parameter.
**/
void send_to_all_players(game_t* game, player_t* excluding, const char* fmt, ...) {
  assert(game);
  assert(game->players);

  char output[SEND_SIZE];

  h_it_t it = hash_table_iterator(game->players);
  player_t* target;

  va_list args;
  va_start(args, fmt);

  if ((vsnprintf(output, SEND_SIZE, fmt, args)) >= SEND_SIZE) {
    mlog(ERROR, "send_to_all_players", "Formatted output was too long and was truncated");
  }

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
void write_to_player(player_t* player, char* output) {
  assert(player);
  assert(output);

  if (player->client == NULL) {
    char username[USERNAME_SIZE];
    get_player_username(player, username);

    mlog(WARN, "write_to_player", "Send to player with username [%s] failed as they have no client", username);

    return;
  }

  char* chosen_output = output;
  char ansi_output[SEND_SIZE];

  if (convert_symbols_to_ansi_codes(output, ansi_output, SEND_SIZE) == 0) {
    chosen_output = ansi_output;
  }

  if (send_to_client(player->client, chosen_output) != 0) {
    mlog(WARN, "write_to_player", "Send to player failed, unable to write to client [%s]", player->client->uuid);

    return;
  }
}

/**
 * Copies the players username into the buffer pointed to by username. If
 * the player does not have a username yet, it is replaced with a placeholder.
**/
void get_player_username(player_t* player, char* username) {
  assert(player);
  assert(username);

  strncpy(username, player->account->username != NULL ? player->account->username : "anonymous", USERNAME_SIZE);
}
