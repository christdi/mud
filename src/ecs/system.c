#include "mud/ecs/system.h"
#include "mud/ecs/description.h"
#include "mud/game.h"

/**
 * Runs the various sytems responsible for updating the various components.
 * This method should be called once per engine update.  
**/
void update_systems(game_t* game) {
  update_description(game);
}