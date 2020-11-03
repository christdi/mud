#include <assert.h>
#include <string.h>

#include "mud/command/command.h"
#include "mud/command/admin.h"
#include "mud/data/hash_table/hash_iterator.h"
#include "mud/ecs/character_details.h"
#include "mud/util/mudstring.h"
#include "mud/player.h"
#include "mud/game.h"


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
void entity_command(player_t * player, game_t * game, char * input) {
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

    entity_t * entity;

    send_to_player(player, "\n\r[cyan]Entities[reset]\n\n\r");

    while ((entity = (entity_t *) h_it_get(it)) != NULL) {
      send_to_player(player, "[white]%s[reset]\n\r", entity->uuid);

      if (has_character_details(game->components, entity)) {
        send_to_player(player, "- [green]character details[reset]\n\r");
      } else {
        send_to_player(player, "- [red]character details[reset]\n\r");
      }

      it = h_it_next(it);
    }
  }

  if (strncmp("assign", subcommand, ARGUMENT_SIZE) == 0) {
    char entity_uuid[UUID_SIZE];
    input = extract_argument(input, entity_uuid);

    if (*entity_uuid == '\0') {
      send_to_player(player, "\n\rSyntax: entity assign <uuid>\n\r");

      return;
    }

    entity_t * entity;

    if ((entity = get_entity(game, entity_uuid)) == NULL) {
      send_to_player(player, "\nNo entity with uuid [cyan]%s[reset] found.\n\r", entity_uuid);

      return;
    }

    if (entity->player != NULL) {
      send_to_player(player, "\n\rEntity [cyan]%s[reset] is already assigned to [cyan]%s[reset].\n\r", entity_uuid, entity->player->username);

      return;
    }

    send_to_player(player, "\n\rAssigning entity uuid [cyan]%s[reset] to you.\n\r", entity->uuid);

    assign_entity(entity, player);
  }


}