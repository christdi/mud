#include "mud/narrator/narrator.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/character_details.h"
#include "mud/game.h"
#include "mud/player.h"

/**
 * Narrates an entity speaking to any entities whom have a player.
**/
void narrate_on_speak(game_t * game, entity_t * entity, char * what) {
	if (entity->player) {
		send_to_player(entity->player, "\n\rYou say '[cyan]%s[reset]'.\n\r", what);
	}

	character_details_t * details = get_character_details(game->components, entity);

	send_to_all_players(game, entity->player, "\n\r[cyan]%s[reset] says '[cyan]%s[reset]'.\n\r", details->name, what);
}