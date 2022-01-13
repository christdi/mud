#include "mud/player.h"
#include "mud/account.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/db/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/lua/repository.h"
#include "mud/lua/script.h"
#include "mud/network/client.h"
#include "mud/state/state.h"
#include "mud/util/mudstring.h"

#include <assert.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int call_state_exit_function(state_t* state, player_t* player, game_t* game);
static int call_state_enter_function(state_t* state, player_t* player, game_t* game);
static void get_player_username(player_t* player, char* username);
static void write_to_player(player_t* player, char* output);

/**
 * Allocates and initialises a new player_t struct.
 *
 * Returns the allocated player_t struct.
**/
player_t* create_player_t() {
  player_t* player = calloc(1, sizeof *player);

  player->uuid = new_uuid();
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

  hash_table_insert(game->players, uuid_str(&client->uuid), player);

  lua_hook_on_player_connected(game->lua_state, player);

  player_change_state(player, game, "login");
}

/**
 * Callback from the network module when a client disconnects.
**/
void player_disconnected(client_t* client, void* context) {
  game_t* game = (game_t*)context;

  player_t* player = hash_table_get(game->players, uuid_str(&client->uuid));

  lua_hook_on_player_disconnected(game->lua_state, player);

  hash_table_delete(game->players, uuid_str(&client->uuid));
}

/**
 * Callback from the network module when a client receives input.
**/
void player_input(client_t* client, void* context) {
  game_t* game = (game_t*)context;

  player_t* player = hash_table_get(game->players, uuid_str(&client->uuid));

  char command[COMMAND_SIZE];

  while (extract_from_input(client, command, sizeof(command), "\r\n") != -1) {
    if (strnlen(command, sizeof(command) - 1) > 0) {
      lua_hook_on_player_input(game->lua_state, player, command);

      if (player->state != NULL) {
      }
    }
  }
}

/**
 * Function which attempts to find a state from persistence and assign it to the player.
 *
 * Parameters
 *   player - the player whose state is to be changed
 *   game - the game struct
 *   state - the name of the state to be found
**/
int player_change_state(player_t* player, game_t* game, const char* state) {
  state_t* new_state = create_state_t();

  if (db_state_load_by_name(game->database, state, new_state) != 1) {
    LOG(ERROR, "Unable to change player state to [%s] as it was not in database", state);

    free_state_t(new_state);

    return -1;
  }

  state_t* old_state = player->state;
  player->state = new_state;

  if (call_state_exit_function(old_state, player, game) == -1) {
    LOG(WARN, "Error encountered while running on_exit for state [%s]", old_state->name);
  }

  if (call_state_enter_function(player->state, player, game) == -1) {
    LOG(WARN, "Error encountered while running on_enter for state [%s]", player->state->name);
  }

  if (old_state != NULL) {
    free_state_t(old_state);
  }
  
  return 0;
}

/**
 * Attempts to load and run the on_exit method of a state.
 * 
 * Parameters
 *   state - state for which to run on_exit
 *   player - the player whose state it is
 *   game - general game data
 * 
 * Returns 0 on success or -1 on failure
**/
static int call_state_exit_function(state_t* state, player_t* player, game_t* game) {
  assert(player);
  assert(game);

  if (state == NULL || state->on_exit == NULL) {
    return 0;
  }

  const char* script_uuid = uuid_str(&state->script);
  script_t* script = NULL;

  if (script_repository_load(game->scripts, game, script_uuid, &script) == -1) {
    LOG(ERROR, "Unable to load script uuid [%s]", script_uuid);

    return -1;
  }

  if (script_call_state_exit(script, state, player) == -1) {
    LOG(ERROR, "Error calling state script with uuid [%s] on_exit function", script_uuid);

    return -1;
  }

  return 0;
}

/**
 * Attempts to load and run the on_enter method of a state.
 * 
 * Parameters
 *   state - state for which to run on_exit
 *   player - the player whose state it is
 *   game - general game data
 * 
 * Returns 0 on success or -1 on failure
**/
static int call_state_enter_function(state_t* state, player_t* player, game_t* game) {
  assert(player);
  assert(game);

  if (state == NULL || state->on_enter == NULL) {
    return 0;
  }

  const char* script_uuid = uuid_str(&state->script);
  script_t* script = NULL;

  if (script_repository_load(game->scripts, game, script_uuid, &script) == -1) {
    LOG(ERROR, "Unable to load script uuid [%s]", script_uuid);

    return -1;
  }

  if (script_call_state_enter(script, state, player) == -1) {
    LOG(ERROR, "Error calling state script with uuid [%s] on_enter function", script_uuid);

    return -1;
  }

  return 0;
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
    const char* script_uuid = uuid_str(&player->state->script);

    script_t* script = NULL;

    if (script_repository_load(game->scripts, game, script_uuid, &script) == -1) {
      LOG(ERROR, "Unable to load script uuid [%s]", script_uuid);

      return;
    }

    if (script_call_state_tick(script, player->state, player) == -1) {
      LOG(ERROR, "Error calling state script with uuid [%s] on_tick function", script_uuid);

      return;
    }
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
    LOG(ERROR, "Formatted output was too long and was truncated");
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
    LOG(ERROR, "Formatted output was too long and was truncated");
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
    LOG(ERROR, "Formatted output was too long and was truncated");
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
static void write_to_player(player_t* player, char* output) {
  assert(player);
  assert(output);

  if (player->client == NULL) {
    char username[USERNAME_SIZE];
    get_player_username(player, username);

    LOG(WARN, "Send to player with username [%s] failed as they have no client", username);

    return;
  }

  char* chosen_output = output;
  char ansi_output[SEND_SIZE];

  if (convert_symbols_to_ansi_codes(output, ansi_output, SEND_SIZE) == 0) {
    chosen_output = ansi_output;
  }

  if (send_to_client(player->client, chosen_output) != 0) {
    LOG(WARN, "Send to player failed, unable to write to client [%s]", player->client->uuid);

    return;
  }
}

/**
 * Copies the players username into the buffer pointed to by username. If
 * the player does not have a username yet, it is replaced with a placeholder.
**/
static void get_player_username(player_t* player, char* username) {
  assert(player);
  assert(username);

  strncpy(username, player->account->username != NULL ? player->account->username : "anonymous", USERNAME_SIZE);
}
