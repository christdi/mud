#include "mud/narrator/narrator.h"
#include "mud/ecs/description.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/player.h"

/**
 * Narrates an entity speaking to any entities whom have a player.
**/
void narrate_speak_action(game_t* game, entity_t* entity, char* what) {
  if (entity->player) {
    send_to_player(entity->player, "\n\rYou say '[cyan]%s[reset]'.\n\r", what);
  }

  description_t* description = get_description(game->components, entity);

  send_to_all_players(game, entity->player, "\n\r[cyan]%s[reset] says '[cyan]%s[reset]'.\n\r", description->name, what);
}