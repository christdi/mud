#include <assert.h>
#include <string.h>

#include "mud/account.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/narrator/narrator.h"
#include "mud/network/client.h"
#include "mud/player.h"
#include "mud/state/account_state.h"
#include "mud/state/play_state.h"
#include "mud/state/state.h"
#include "mud/util/mudstring.h"

void account_state_enter(player_t* player, game_t* game);
void account_state_input(player_t* player, game_t* game, char* input);
entity_t* account_state_get_entity_by_name(game_t* game, linked_list_t* entities, const char* name);

/**
 * Creates and configures a new state_t for the account state.  It's the responsibility
 * of the caller to ensure the allocated state is freed.
 *
 * Returns the allocated state.
**/
state_t* account_state() {
  state_t* state = create_state_t();

  state->on_enter = account_state_enter;
  state->on_input = account_state_input;

  return state;
}

/**
 * Entry point for the account state.
 *
 * Parameters
 *  player - the player entering this state
 *  game - game object containing game dependencies
**/
void account_state_enter(player_t* player, game_t* game) {
  assert(player);
  assert(game);

  send_to_player(player, "--------------------------------------------------------------------------------\n\r");
  send_to_player(player, "| Account: %s\n\r", player->account->username);
  send_to_player(player, "--------------------------------------------------------------------------------\n\r");
  send_to_player(player, "Your account can play:\n\n\r");

  it_t it = list_begin(player->account->entities);

  char* entity_uuid = NULL;

  while ((entity_uuid = (char*)it_get(it)) != NULL) {
    entity_t* entity = get_entity(game, entity_uuid);

    if (entity != NULL) {
      send_to_player(player, "  * %s\n\r", entity->name);
    }

    it = it_next(it);
  }
  
  send_to_player(player, "\n\rEnter a [bgreen]character name[reset] to select a character, [bgreen]new[reset] to create a new character or [bgreen]quit[reset].\n\r");
}

/**
 * State which accepts input for the account state.
 *
 * Parameters
 *  player - the player entering this state
 *  game - game object containing game dependencies
**/
void account_state_input(player_t* player, game_t* game, char* input) {
  char command[ARGUMENT_SIZE];
  input = extract_argument(input, command, sizeof(command));

  if (strcmpi(command, "new") == 0) {
    char name[ARGUMENT_SIZE];
    extract_argument(input, name, sizeof(name));

    if (name[0] == '\0') {
      send_to_player(player, "Syntax: new <name>\n\r");
      return;
    }

    entity_t* entity = new_entity(game, name, "Player created character entity");
    player->entity = entity;


    if (add_player_to_narration(game->narrator, entity, player) != 0) {
      player->entity = NULL;

      mlog(ERROR, "entity_command", "Unable to add player [%s] to narration of entity [%s]", player->account->username, entity->id.uuid);
      send_to_player(player, "We couldn't assign you to %s, please inform an administrator\n\r", name);
      send_to_player(player, "\n\rEnter a [bgreen]character name[reset] to select a character, [bgreen]new[reset] to create a new character or [bgreen]quit[reset].\n\r");
      return;
    };

    player_change_state(player, game, play_state());
    return;
  }

  if (strcmpi(command, "quit") == 0) {
    player->client->hungup = 1;

    return;
  }

  entity_t* entity = account_state_get_entity_by_name(game, player->account->entities, command);

  if (entity != NULL) {
    player->entity = entity;

    if (add_player_to_narration(game->narrator, entity, player) != 0) {
      player->entity = NULL;

      mlog(ERROR, "entity_command", "Unable to add player [%s] to narration of entity [%s]", player->account->username, entity->id.uuid);
      send_to_player(player, "We couldn't assign you to %s, please inform an administrator\n\r", entity->name);
      send_to_player(player, "\n\rEnter a [bgreen]character name[reset] to select a character, [bgreen]new[reset] to create a new character or [bgreen]quit[reset].\n\r");     
      return;
    };

    player_change_state(player, game, play_state());
    return;
  }  

  send_to_player(player, "\n\rEnter a [bgreen]character name[reset] to select a character, [bgreen]new[reset] to create a new character or [bgreen]quit[reset].\n\r");
}

/**
 * Retrieves an entity matching a given name from a linked list
 * 
 * Parameters
 *  entities - the linked list to search for the entity
 *  name - the name to search for
 * 
 * Returns a pointer to the entity
**/
entity_t* account_state_get_entity_by_name(game_t* game, linked_list_t* entities, const char* name) {
  it_t it = list_begin(entities);

  char* entity_uuid = NULL;

  while ((entity_uuid = (char*)it_get(it)) != NULL) {
    entity_t* entity = get_entity(game, entity_uuid);

    if (entity != NULL) {
      mlog(INFO, "account_state_get_entity_by_name", "Comparing %s and %s", entity->name, name);
      if (strcmpi(entity->name, name) == 0) {
        mlog(INFO, "account_state_get_entity_by_name", "Match!");
        return entity;
      }
      
    }

    it = it_next(it);
  }

  return NULL;
}
