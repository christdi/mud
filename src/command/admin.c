#include <assert.h>
#include <string.h>

#include "mud/account.h"
#include "mud/command/admin.h"
#include "mud/command/command.h"
#include "mud/data/hash_table.h"
#include "mud/db/db.h"
#include "mud/ecs/component/location.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/narrator/narrator.h"
#include "mud/player.h"
#include "mud/template.h"
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
    send_to_player(player, tpl(game->templates, "command.entity.usage"));

    return;
  }

  char subcommand[ARGUMENT_SIZE];
  input = extract_argument(input, subcommand, sizeof(subcommand));

  if (strncmp("list", subcommand, ARGUMENT_SIZE) == 0) {
    h_it_t it = hash_table_iterator(game->entities);

    entity_t* entity = NULL;

    send_to_player(player, tpl(game->templates, "command.entity.list.header"));

    while ((entity = (entity_t*)h_it_get(it)) != NULL) {
      send_to_player(player, tpl(game->templates, "command.entity.description"), entity->id.uuid, entity->name, entity->description);

      location_t* location = get_location(game->components, entity);

      if (location) {
        char buffer[BUFFER_SIZE];
        describe_location(location, buffer, BUFFER_SIZE);
        send_to_player(player, tpl(game->templates, "command.entity.list.location"), buffer);
      }

      send_to_player(player, "\n\r");

      it = h_it_next(it);
    }
  }

  if (strncmp("assign", subcommand, ARGUMENT_SIZE) == 0) {
    char entity_uuid[UUID_SIZE];
    extract_argument(input, entity_uuid, sizeof(entity_uuid));

    if (*entity_uuid == '\0') {
      send_to_player(player, tpl(game->templates, "command.entity.assign.usage"));

      return;
    }

    entity_t* entity = NULL;

    if ((entity = get_entity(game, entity_uuid)) == NULL) {
      send_to_player(player, tpl(game->templates, "command.no.entity"), entity_uuid);

      return;
    }

    send_to_player(player, tpl(game->templates, "command.entity.assign.success"), entity->id.uuid);

    if (player->entity != NULL) {
      if (remove_player_from_narration(game->narrator, player->entity, player) != 0) {
        mlog(ERROR, "entity_command", "Unable to remove player [%s] from narration of entity [%s]", player->account->username, entity->id.uuid);
        send_to_player(player, tpl(game->templates, "command.entity.assign.narration.remove.failure"), entity->id.uuid);
      }
    }

    player->entity = entity;

    if (add_player_to_narration(game->narrator, entity, player) != 0) {
      mlog(ERROR, "entity_command", "Unable to add player [%s] to narration of entity [%s]", player->account->username, entity->id.uuid);
      send_to_player(player, tpl(game->templates, "command.entity.assign.narration.remove.success"), entity->id.uuid);
    };
  }

  if (strncmp("create", subcommand, ARGUMENT_SIZE) == 0) {
    char name[ARGUMENT_SIZE];
    input = extract_argument(input, name, sizeof(name));
    char description[ARGUMENT_SIZE];
    input = extract_argument(input, description, sizeof(description));

    if (name[0] == '\0' || description[0] == '\0') {
      send_to_player(player, tpl(game->templates, "command.entity.create.usage"));
      return;
    }

    entity_t* create_entity = new_entity(game, name, description);
    send_to_player(player, tpl(game->templates, "command.entity.create.success"), create_entity->id.uuid, create_entity->name, create_entity->description);
  }

  if (strncmp("save", subcommand, ARGUMENT_SIZE) == 0) {
    char entity_id[UUID_SIZE];

    extract_argument(input, entity_id, sizeof(entity_id));

    if (*entity_id == '\0') {
      send_to_player(player, tpl(game->templates, "command.entity.save.usage"));

      return;
    }

    entity_t* entity = NULL;

    if ((entity = get_entity(game, entity_id)) == NULL) {
      send_to_player(player, tpl(game->templates, "command.no.entity"), entity_id);

      return;
    }

    if (db_entity_save(game->database, entity) != 0) {
      send_to_player(player, tpl(game->templates, "command.entity.save.failure"), entity->id.uuid, entity->name, entity->description);

      return;
    }

    send_to_player(player, tpl(game->templates, "command.entity.save.success"), entity->id.uuid, entity->name, entity->description);
  }
}
