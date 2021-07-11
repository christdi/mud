#include <assert.h>

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/narrator/narrator.h"
#include "mud/ecs/description.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/player.h"

narrator_t* create_narrator_t() {
  narrator_t* narrator = calloc(1, sizeof *narrator);

  narrator->entities = create_hash_table_t();

  return narrator;
}

void free_narrator_t(narrator_t* narrator) {
  assert(narrator);

  if (narrator->entities != NULL) {
    free_hash_table_t(narrator->entities);
  }

  free(narrator);
}

int add_player_to_narration(narrator_t* narrator, entity_t* entity, player_t *player) {
  assert(narrator);
  assert(entity);
  assert(player);

  linked_list_t * players = NULL;

  if (hash_table_has(narrator->entities, entity->uuid)) {
    players = (linked_list_t*)hash_table_get(narrator->entities, entity->uuid);
  } else {
    players = create_linked_list_t();

    if (hash_table_insert(narrator->entities, entity->uuid, players) != 0) {
      zlog_error(gc, "Failed to add player to entity [%s] narration", entity->uuid);

      return -1;
    };
  }

  list_add(players, player);

  return 0;
}

int remove_player_from_narration(narrator_t* narrator, entity_t* entity, player_t* player) {
  assert(narrator);
  assert(entity);
  assert(player);

  if (!hash_table_has(narrator->entities, entity->uuid)) {
    zlog_error(gc, "Failed to remove player from entity narration as entity [%s] has no listeners", entity->uuid);

    return -1;
  }

  linked_list_t * players = (linked_list_t*)hash_table_get(narrator->entities, entity->uuid);

  list_remove(players, player);

  return 0;
}

int retrieve_entity_listeners(narrator_t* narrator, entity_t* entity, linked_list_t** listeners) {
  assert(narrator);
  assert(entity);
  assert(listeners);

  if (!hash_table_has(narrator->entities, entity->uuid)) {
    *listeners = NULL;

    return -1;
  }

  *listeners = (linked_list_t*)hash_table_get(narrator->entities, entity->uuid);

  return 0;
}

/**
 * Narrates an entity speaking to any entities whom have a player.
**/
void narrate_speak_action(game_t* game, entity_t* entity, char* what) {
  assert(game);
  assert(entity);
  assert(what);

  linked_list_t * origin_player = NULL;

  if (retrieve_entity_listeners(game->narrator, entity, &origin_player) != 0) {
    return;
  }

  it_t origin_it = list_begin(origin_player);

  player_t* player = NULL;

  while ((player = (player_t*)it_get(origin_it)) != NULL) {

  }

  if (entity->player) {
    send_to_player(entity->player, "\n\rYou say '[cyan]%s[reset]'.\n\r", what);
  }

  description_t* description = get_description(game->components, entity);

  send_to_all_players(game, entity->player, "\n\r[cyan]%s[reset] says '[cyan]%s[reset]'.\n\r", description->name, what);
}
