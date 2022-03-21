#include "mud/player.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/db.h"
#include "mud/event.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/lua/script.h"
#include "mud/network/client.h"
#include "mud/network/gmcp.h"
#include "mud/network/telnet.h"
#include "mud/state.h"
#include "mud/util/mudhash.h"
#include "mud/util/mudstring.h"
#include "mud/util/muduuid.h"

#include <arpa/telnet.h>
#include <assert.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void write_to_player(player_t* player, char* output);

/**
 * Allocates and initialises a new player_t struct.
 *
 * Returns the allocated player_t struct.
 **/
player_t* create_player_t() {
  player_t* player = calloc(1, sizeof *player);

  player->uuid = new_uuid();
  player->username = NULL;
  player->state = NULL;
  player->client = NULL;

  return player;
}

/**
 * Frees a player_t struct.
 **/
void free_player_t(player_t* player) {
  assert(player);

  if (player->username != NULL) {
    free(player->username);
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

  protocol_t* telnet = network_new_telnet_protocol_t();
  network_register_telnet_extension(telnet->data, network_new_gmcp_telnet_extension());

  network_add_client_protocol(client, telnet);

  player_t* player = create_player_t();
  player->client = client;
  client->userdata = player;

  hash_table_insert(game->players, uuid_str(&player->uuid), player);

  lua_call_player_connected_hook(game->lua_state, player);
}

/**
 * Callback from the network module when a client disconnects.
 **/
void player_disconnected(client_t* client, void* context) {
  game_t* game = (game_t*)context;
  player_t* player = client->userdata;

  lua_call_player_disconnected_hook(game->lua_state, player);
  hash_table_delete(game->players, uuid_str(&player->uuid));
}

/**
 * Callback from the network module when a client receives input.
 **/
void player_input(client_t* client, void* context) {
  game_t* game = (game_t*)context;
  player_t* player = client->userdata;

  char command[COMMAND_SIZE];

  while (extract_from_input(client, command, sizeof(command), "\r\n") != -1) {
    if (strnlen(command, sizeof(command) - 1) > 0) {
      lua_call_player_input_hook(game->lua_state, player, command);
      lua_call_state_input_hook(game->lua_state, player, player->state, command);
    }
  }
}

/**
 * Callback from the network module to indicate this client is about to be flushed.
 **/
void player_output(client_t* client, void* context) {
  game_t* game = (game_t*)context;
  player_t* player = client->userdata;

  if (lua_call_state_output_hook(game->lua_state, player, player->state, client->output) == -1) {
    LOG(ERROR, "Error calling state output hook");
  };
}

/**
 * Function which attempts to find a state from persistence and assign it to the player.
 *
 * Parameters
 *   player - the player whose state is to be changed
 *   game - the game struct
 *   state - the name of the state to be found
 **/
int player_change_state(player_t* player, game_t* game, state_t* state) {
  assert(player);
  assert(game);
  assert(state);

  state_t* old_state = player->state;
  player->state = state;

  if (old_state != NULL) {
    lua_call_state_exit_hook(game->lua_state, player, old_state);
  }

  lua_call_state_enter_hook(game->lua_state, player, player->state);

  return 0;
}

/**
 * Called when an event occurs so that the players narrator can evaluate the event and
 * send output to the player if relevant.
 *
 * Parameters
 *  player - the player who is receiving the event
 *  game - instance of game_t containing data required by downstream calls
 *  event - the event that has occurred
 **/
void player_on_event(player_t* player, game_t* game, event_t* event) {
  assert(player);
  assert(game);
  assert(event);

  lua_call_state_event_hook(game->lua_state, player, player->state, event);
}

/**
 * Authenticates a player by hashing their password and comparing the supplied username
 * and password hash against users in the database.  If authentication is successful,
 * the player struct is populated with user details.
 *
 * Parameters
 *   player - the player to be authenticated
 *   username - the username to be authenticated
 *   password - the password to be authenticated
 *
 * Returns 0 on success or -1 on failure
 **/
int player_authenticate(player_t* player, game_t* game, const char* username, const char* password) {
  assert(player);
  assert(username);
  assert(password);

  char password_hash[SHA256_HEX_SIZE];
  mudhash_sha256(password, password_hash);

  if (db_user_authenticate(game->database, username, password_hash) != 1) {
    return -1;
  }

  if (db_user_load_by_username(game->database, username, player) != 1) {
    return -1;
  }

  return 0;
}

/**
 * Narrates events that have occurred in the world if they are relevant to the player.
 *
 * Parameters
 *   player - The player whom should be narrated to
 *   game - instance of game_t containing data required by downstream calls
 *   event - The event that has occurred
 *
 * Returns 0 on success or -1 on failure
 **/
int player_narrate(player_t* player, game_t* game, event_t* event) {
  assert(player);
  assert(game);
  assert(event);

  switch (event->type) {
  case LUA_EVENT:
    lua_call_narrate_event_hook(game->lua_state, player, player->narrator, event->data);
    break;
  default:
    send_to_player(player, "Something happened but you're not sure how to describe it.\n\r");
    break;
  }

  return 0;
}

/**
 * Request the player disables their echo so their input is not displayed, i.e. passwords.
 * This is accomplished by using the telnet protocol to tell the client we will echo and
 * then we just don't.  This method is a no-op if the player does not have a telnet client.
 * 
 * player - the player who we are requesting disable their echo
 *
 * Returns 0 on success
**/
int player_request_disable_echo(player_t* player) {
  if (network_client_has_protocol(player->client, TELNET)) {
    telnet_t* telnet = network_client_get_protocol(player->client, TELNET);

    network_telnet_send_will(telnet, player->client, TELOPT_ECHO);
  }

  return 0;
}

/**
 * Request the player enable their echo so their input. This is accomplished by using the telnet 
 * protocol to tell the client we won't echo.  This method is a no-op if the player does not have 
 * a telnet client.
 * 
 * player - the player who we are requesting enable their echo
 *
 * Returns 0 on success
**/
int player_request_enable_echo(player_t* player) {
  if (network_client_has_protocol(player->client, TELNET)) {
    telnet_t* telnet = network_client_get_protocol(player->client, TELNET);

    network_telnet_send_wont(telnet, player->client, TELOPT_ECHO);
  }

  return 0;
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

/**
 * Attempts to send a GMCP message to a player.  If the player client does not have
 * the telnet protocol with the GMCP extension this is a no-op.
 *
 * player - player to send the gmcp message to
 * topic - null terminated string containing the topic of the gmcp message
 * msg - null terminated string containing the msg of the gmcp message, may be NULL
**/
void send_gmcp_to_player(player_t* player, char* topic, char* msg) {
  assert(player);
  assert(topic);

  if (!network_client_has_protocol(player->client, TELNET)) {
    return;
  }

  size_t topic_len = strnlen(topic, TOPIC_LEN);
  size_t msg_len = msg != NULL ? strnlen(msg, MSG_LEN) : 0;

  network_send_gmcp_message(player->client, topic, topic_len, msg, msg_len);
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
    LOG(WARN, "Could not write to player [%s] as client has disconnected", player->uuid);

    return;
  }

  char* chosen_output = output;
  char ansi_output[SEND_SIZE];

  if (convert_symbols_to_ansi_codes(output, ansi_output, SEND_SIZE) == 0) {
    chosen_output = ansi_output;
  }

  size_t len = strnlen(chosen_output, SEND_SIZE);

  if (send_to_client(player->client, chosen_output, len) != 0) {
    LOG(WARN, "Send to player failed, unable to write to client [%s]", uuid_str(&player->uuid));

    return;
  }
}
