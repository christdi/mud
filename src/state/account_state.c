#include <string.h>

#include "mud/account.h"
#include "mud/state/account_state.h"
#include "mud/player.h"
#include "mud/state/play_state.h"
#include "mud/state/state.h"

void account_state_enter(player_t* player, game_t* game);
void account_state_input(player_t* player, game_t* game, char* input);

/**
 * Creates and configures a new state_t for the account state.  It's the responsibility
 * of the caller to ensure the allocated state is freed.
 *
 * Returns the allocated state.
**/
state_t* account_state() {
  state_t* state = create_state_t();

  state->on_enter = account_state_enter;
  state->on_input = account_state_input;

  return state;
}

/**
 * Entry point for the account state.
 *
 * Parameters
 *  player - the player entering this state
 *  game - game object containing game dependencies
**/
void account_state_enter(player_t* player, game_t* game) {
  send_to_player(player, "--------------------------------------------------------------------------------\n\r");
  send_to_player(player, "| Account: %s\n\r", player->account->username);
  send_to_player(player, "--------------------------------------------------------------------------------\n\r");
  send_to_player(player, "Your account has access to the following characters:\n\n\r");
  // TODO(Chris) - Display entities user has access to
  send_to_player(player, "Enter [bgreen]play <name>[reset] to select a character or [bgreen]new[reset] to create a new character.\n\r");
}

/**
 * State which accepts input for the account state.
 *
 * Parameters
 *  player - the player entering this state
 *  game - game object containing game dependencies
**/
void account_state_input(player_t* player, game_t* game, char* input) {
  if (strcmp(input, "play") == 0) {
    player_change_state(player, game, play_state());

    return;
  }

  send_to_player(player, "Enter [bgreen]play <name>[reset] to select a character or [bgreen]new[reset] to create a new character.\n\r");
}
