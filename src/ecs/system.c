#include "mud/ecs/system.h"
#include "mud/ecs/character_details.h"
#include "mud/game.h"


/**
 * Runs the various sytems responsible for updating the various components.
 * This method should be called once per engine update.  
**/
void update_systems(game_t * game) {
	update_character_details(game);
}