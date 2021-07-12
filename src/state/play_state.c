#include <assert.h>

#include "mud/command/command.h"
#include "mud/command/command_function.h"
#include "mud/data/linked_list.h"
#include "mud/data/hash_table.h"
#include "mud/dbo/account.h"
#include "mud/dbo/command_dbo.h"
#include "mud/ecs/component/description.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/player.h"
#include "mud/state/play_state.h"
#include "mud/util/mudstring.h"

void send_prompt(player_t* player, game_t* game);

/**
 * State method for players who are currently in the game world.
**/
void play_state(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);

  if (!input) {
    send_to_all_players(game, NULL, "\n\r[bcyan]%s[reset] has entered the world.\n\r", player->account->username);
    send_prompt(player, game);

    return;
  }

  char command[COMMAND_SIZE];
  input = extract_argument(input, command);

  linked_list_t* commands = create_linked_list_t();
  commands->deallocator = deallocate_command_dbo_t;

  int count = 0;

  if ((count = get_commands_by_name(game, trim(command), commands)) == -1) {
    zlog_error(gc, "Error retrieving commands");
  }

  if (count == 0) {
    send_to_player(player, "[bcyan]%s[reset] command not recognised.\n\r", command);
    send_prompt(player, game);

    return;
  }

  if (count > 0) {
    command_dbo_t* command_dbo = NULL;
    list_at(commands, 0, (void*)&command_dbo);

    if (command_dbo == NULL) {
      zlog_error(gc, "command_dbo was null");
      return;
    }

    command_function_t* cmd = (command_function_t*)hash_table_get(game->commands, command_dbo->function);

    if (cmd != NULL) {
      cmd->func(player, game, trim(input));
    }
  }



  send_prompt(player, game);

  free_linked_list_t(commands);
}

/**
 * Send a prompt to a player.
**/
void send_prompt(player_t* player, game_t* game) {
  if (player->entity) {
    description_t* description = get_description(game->components, player->entity);

    if (description) {
      send_to_player(player, "[bgreen]%s[reset] => ", description->name);
      return;
    }
  }

  send_to_player(player, "[bgreen]%s[reset] => ", player->account->username);
}
