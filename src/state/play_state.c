#include <assert.h>

#include "mud/command/command.h"
#include "mud/ecs/component/description.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/player.h"
#include "mud/state/play_state.h"
#include "mud/state/state.h"
#include "mud/util/mudstring.h"

void send_prompt(player_t* player, game_t* game);
void play_state_enter(player_t* player, game_t* game);
void play_state_input(player_t* player, game_t* game, char* input);
void play_state_tick(player_t* player, game_t* game);

/**
 * Creates a state_t configured for the play state
 * 
 * Returns the configured state_t
**/
state_t* play_state() {
  state_t* state = create_state_t();

  state->on_enter = play_state_enter;
  state->on_input = play_state_input;
  state->on_tick = play_state_tick;

  return state;
}

/**
 * Method called when the player initially enters the play state.
 * 
 * Parameters
 *  player - player_t representing the player whose input we are parsing
 *  game - game_t providing access to general game data 
**/
void play_state_enter(player_t* player, game_t* game) {
  assert(player);
  assert(game);

  send_to_all_players(game, NULL, "\n\r[bcyan]%s[reset] has entered the world!\n\r", player->username);
}

/**
 * Parses input from the player when they are in the play state.
 * 
 * Parameters
 *  player - player_t representing the player whose input we are parsing
 *  game - game_t providing access to general game data
 *  input - the input received from the player
**/
void play_state_input(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);

  char command[COMMAND_SIZE];
  input = extract_argument(input, command);

  const command_t* cmd = get_command(game, trim(command));

  if (cmd == NULL) {
    send_to_player(player, "Sorry, [bcyan]%s[reset] command was not found.\n\r", command);

    return;
  }

  cmd->func(player, game, trim(input));
}

/**
 * Checks the idle time of the player and sends the prompt if they've been idle for
 * too long.
 * 
 * Parameters
 *  player - the player being ticked
 *  game - game_t struct containing game data
**/
void play_state_tick(player_t* player, game_t* game) {
  assert(player);
  assert(game);
  assert(player->client);

  send_prompt(player, game);
}

/**
 * Send a prompt to a player.
**/
void send_prompt(player_t* player, game_t* game) {
  assert(player);
  assert(game);

  if (player->entity) {
    description_t* description = get_description(game->components, player->entity);

    if (description) {
      send_to_player(player, "\n\r<[bgreen]%s[reset]>\n\r", description->name);
      return;
    }
  }

  send_to_player(player, "\n\r<[bgreen]%s[reset]>\n\r", player->username);
}
