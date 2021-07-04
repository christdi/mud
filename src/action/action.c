#include "mud/action/action.h"
#include "mud/action/action_callback.h"
#include "mud/data/hash_table.h"
#include "mud/ecs/description.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/narrator/narrator.h"

/**
 * Actiom which allows an entity to speak.
**/
void speak_action(entity_t* entity, game_t* game, char* what) {
  description_t* description = get_description(game->components, entity);

  if (!description) {
    zlog_warn(gc, "Speak action attempted on entity [%s] which does not have character details", entity->uuid);

    return;
  }

  entity_t* target = NULL;

  h_it_t it = hash_table_iterator(game->entities);

  while ((target = (entity_t*)h_it_get(it)) != NULL) {
    if (target->action_callback->on_speak != NULL) {
      target->action_callback->on_speak(game, entity, what);
    }

    it = h_it_next(it);
  }

  narrate_speak_action(game, entity, what);
}
