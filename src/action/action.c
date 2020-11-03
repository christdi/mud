#include "mud/action/action.h"
#include "mud/action/action_callback.h"
#include "mud/ecs/character_details.h"
#include "mud/narrator/narrator.h"
#include "mud/data/hash_table.h"
#include "mud/log.h"
#include "mud/game.h"


/**
 * Actiom which allows an entity to speak.
**/
void speak_action(entity_t * entity, game_t * game, char * what) {
	character_details_t	* character_details = get_character_details(game->components, entity);

	if (character_details == NULL) {
		zlog_warn(gc, "Speak action attempted on entity [%s] which does not have character details", entity->uuid);

		return;
	}

	entity_t * target;

	h_it_t it = hash_table_iterator(game->entities);

	while ((target = (entity_t *) h_it_get(it)) != NULL) {
		if (target->action_callback->on_speak != NULL) {
			target->action_callback->on_speak(game, entity, what);
		}

		it = h_it_next(it);
	}

	narrate_on_speak(game, entity, what);
}