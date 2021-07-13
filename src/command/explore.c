#include <assert.h>

#include "mud/command/explore.h"
#include "mud/data/hash_table.h"
#include "mud/dbo/account.h"
#include "mud/ecs/component/description.h"
#include "mud/ecs/component/location.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/player.h"

/**
 * Command which tells a player about what is contained within their current entity.
**/
void inventory_command(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);
  assert(input);

  entity_t* entity = player->entity;

  if (!entity) {
    mlog(ERROR, "inventory_command", "Inventory command failed for player [%s] as they do not have an associated entity", player->account->username);

    return;
  }
}

/**
 * Command which tells a player about what is containing their current entity.
**/
void look_command(player_t* player, game_t* game, char* input) {
  assert(player);
  assert(game);
  assert(input);

  entity_t* entity = player->entity;

  if (!entity) {
    mlog(ERROR, "look_command", "Look command failed for player [%s] as they do not have an associated entity", player->account->username);

    return;
  }

  location_t* location_component = get_location(game->components, entity);

  if (!location_component) {
    mlog(ERROR, "look_command", "Look command failed for entity [%s] as it did not have a location component", entity->id.uuid);

    return;
  }

  entity_t* location = get_entity(game, location_component->at.uuid);

  if (!location) {
    mlog(ERROR, "look_command", "Look command failed for entity [%s] as location component does not reference valid entity", entity->id.uuid);

    return;
  }

  description_t* description = get_description(game->components, location);

  if (!description) {
    mlog(ERROR, "look_command", "Look command failed for entity [%s] as entity referenced by location component does not have description component", entity->id.uuid);

    return;
  }

  send_to_player(player, "[cyan]%s[reset]\n\n\r%s\n\r", description->name, description->description);
}
