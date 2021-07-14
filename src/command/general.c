#include "mud/command/general.h"
#include "mud/network/client.h"
#include "mud/player.h"

#include <assert.h>

/**
 * Command which marks a players client as hungup so the game will remove
 * them.
**/
void quit_command(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);

  send_to_player(player, "Goodbye! Thanks for playing\n\r");

  send_to_all_players(game, player, "[bcyan]%s[reset] has left the world.\n\r", player->username);

  if (player && player->client) {
    player->client->hungup = 1;
  }
}
