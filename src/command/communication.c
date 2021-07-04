#include <assert.h>

#include "mud/action/action.h"
#include "mud/command/communication.h"
#include "mud/dbo/account.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/player.h"
#include "mud/util/mudstring.h"

/**
 * Command which broadcasts a statement to all players connected to the game.
**/
void say_command(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);
  assert(input);

  if (!player->entity) {
    zlog_error(gc, "Player [%s] did not have an assigned entity when using the say command", player->account->username);

    return;
  }

  if (*input == '\0') {
    send_to_player(player, "Say what?\n\r");

    return;
  }

  speak_action(player->entity, game, input);
}