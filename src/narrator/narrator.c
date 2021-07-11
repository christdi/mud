#include <assert.h>

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/event/event.h"
#include "mud/event/communicate.h"
#include "mud/narrator/narrator.h"
#include "mud/ecs/component/description.h"
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

  if (hash_table_has(narrator->entities, entity->id.uuid)) {
    players = (linked_list_t*)hash_table_get(narrator->entities, entity->id.uuid);
  } else {
    players = create_linked_list_t();

    if (hash_table_insert(narrator->entities, entity->id.uuid, players) != 0) {
      zlog_error(gc, "Failed to add player to entity [%s] narration", entity->id.uuid);

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

  if (!hash_table_has(narrator->entities, entity->id.uuid)) {
    zlog_error(gc, "Failed to remove player from entity narration as entity [%s] has no listeners", entity->id.uuid);

    return -1;
  }

  linked_list_t * players = (linked_list_t*)hash_table_get(narrator->entities, entity->id.uuid);

  list_remove(players, player);

  return 0;
}

int retrieve_players_for_narration(narrator_t* narrator, entity_t* entity, linked_list_t** listeners) {
  assert(narrator);
  assert(entity);
  assert(listeners);

  if (!hash_table_has(narrator->entities, entity->id.uuid)) {
    *listeners = NULL;

    return -1;
  }

  *listeners = (linked_list_t*)hash_table_get(narrator->entities, entity->id.uuid);

  return 0;
}

/**
 * Narrates an entity speaking to any entities whom have a player.
**/
void narrate_communicate_event(game_t* game, event_t* event) {
  assert(game);
  assert(event);
  assert(event->type == COMMUNICATION);

  communicate_event_t* data = (communicate_event_t*)event->data;

  entity_t* origin_entity = get_entity(game, data->origin.uuid);

  linked_list_t* players = NULL;

  retrieve_players_for_narration(game->narrator, origin_entity, &players);

  if (players != NULL) {
    send_to_players(players, "\n\rYou say '[cyan]%s[reset]'.\n\r", data->what);
  }
}
