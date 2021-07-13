#include <assert.h>
#include <string.h>

#include "mud/command/admin.h"
#include "mud/command/command.h"
#include "mud/data/hash_table.h"
#include "mud/dbo/account.h"
#include "mud/ecs/component/description.h"
#include "mud/ecs/component/location.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/narrator/narrator.h"
#include "mud/player.h"
#include "mud/util/mudstring.h"

void shutdown_command(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);
  assert(input);

  game->shutdown = 1;
}

/**
 * Command which allows a player to assign an entity to themselves.
 *
 * This method takes the following parameters:
 *   player - A pointer to a player struct representing player executing command
 *   game - A pointer to a game struct representing the game
 *   input - A pointer to a null terminated string containing command arguments
 *
 * This method does not return anything.
**/
void entity_command(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);
  assert(input);

  if (*input == '\0') {
    send_to_player(player, "\n\rSyntax: entity <list|assign>\n\r");

    return;
  }

  char subcommand[ARGUMENT_SIZE];
  input = extract_argument(input, subcommand);

  if (strncmp("list", subcommand, ARGUMENT_SIZE) == 0) {
    h_it_t it = hash_table_iterator(game->entities);

    entity_t* entity = NULL;

    send_to_player(player, "\n\r[cyan]Entities in game[reset]\n\n\r");

    while ((entity = (entity_t*)h_it_get(it)) != NULL) {
      send_to_player(player, "[yellow]%s[reset]\n\r", entity->id.uuid);

      description_t* description = get_description(game->components, entity);
      if (description) {
        send_to_player(player, "- [green]description[reset], name => %s\n\r", description->name);
      }

      location_t* location = get_location(game->components, entity);
      if (location) {
        char buffer[BUFFER_SIZE];
        describe_location(location, buffer, BUFFER_SIZE);
        send_to_player(player, "- [green]location[reset], %s\n\r", buffer);
      }

      send_to_player(player, "\n\r");

      it = h_it_next(it);
    }
  }

  if (strncmp("assign", subcommand, ARGUMENT_SIZE) == 0) {
    char entity_uuid[UUID_SIZE];
    extract_argument(input, entity_uuid);

    if (*entity_uuid == '\0') {
      send_to_player(player, "\n\rSyntax: entity assign <uuid>\n\r");

      return;
    }

    entity_t* entity = NULL;

    if ((entity = get_entity(game, entity_uuid)) == NULL) {
      send_to_player(player, "\nNo entity with uuid [cyan]%s[reset] found.\n\r", entity_uuid);

      return;
    }

    send_to_player(player, "\n\rAssigning entity uuid [cyan]%s[reset] to you.\n\r", entity->id.uuid);

    if (player->entity != NULL) {
      if (remove_player_from_narration(game->narrator, player->entity, player) != 0) {
        zlog_error(gc, "entity_command(): Unable to remove player [%s] from narration of entity [%s]", player->account->username, entity->id.uuid);
        send_to_player(player, "\n\rUnable to remove you from narration of entity [%s].\n\r", entity->id.uuid);
      }
    }

    player->entity = entity;

    if (add_player_to_narration(game->narrator, entity, player) != 0) {
      zlog_error(gc, "entity_command(): Unable to add player [%s] to narration of entity [%s]", player->account->username, entity->id.uuid);
      send_to_player(player, "\n\rUnable to add you to narration for entity [%s].\n\r", entity->id.uuid);
    };
  }
}
