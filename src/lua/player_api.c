#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/data/deallocate.h"
#include "mud/data/linked_list.h"
#include "mud/db.h"
#include "mud/ecs/entity.h"
#include "mud/event.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/player_api.h"
#include "mud/lua/struct.h"
#include "mud/network/client.h"
#include "mud/player.h"
#include "mud/util/muduuid.h"

static int lua_authenticate(lua_State* l);
static int lua_narrate(lua_State* l);
static int lua_get_entity(lua_State* l);
static int lua_set_entity(lua_State* l);
static int lua_set_state(lua_State* l);
static int lua_set_narrator(lua_State* l);
static int lua_get_entities(lua_State* l);
static int lua_send_to_player(lua_State* l);
static int lua_disconnect(lua_State* l);
static int lua_disable_echo(lua_State* l);
static int lua_enable_echo(lua_State* l);
static int lua_send_gmcp(lua_State* l);

static const struct luaL_Reg player_lib[] = {
  { "authenticate", lua_authenticate },
  { "narrate", lua_narrate },
  { "get_entity", lua_get_entity },
  { "set_entity", lua_set_entity },
  { "set_state", lua_set_state },
  { "set_narrator", lua_set_narrator },
  { "get_entities", lua_get_entities },
  { "send", lua_send_to_player },
  { "disconnect", lua_disconnect },
  { "disable_echo", lua_disable_echo },
  { "enable_echo", lua_enable_echo },
  { "send_gmcp", lua_send_gmcp },
  { NULL, NULL }
};

/**
 * Registers the player API into a Lua state.
 *
 * Parameters
 *   l - The Lua state to register the API with
 *
 * Returns 0 on success.
 **/
int lua_player_register_api(lua_State* l) {
  luaL_newlib(l, player_lib);
  lua_setglobal(l, "player");

  return 0;
}

/**
 * API function which authenticates the player as a particular user given a username and password.
 *
 * Parameters
 *   l - THe current Lua state
 *
 * Returns 0 on success or calls LuaL_Error on failure.
 **/
static int lua_authenticate(lua_State* l) {
  luaL_checktype(l, -3, LUA_TTABLE);
  player_t* player = lua_to_player(l, -3);

  const char* username = luaL_checkstring(l, -2);
  const char* password = luaL_checkstring(l, -1);

  lua_pop(l, 3);

  game_t* game = lua_common_get_game(l);

  if (player_authenticate(player, game, username, password) == -1) {
    lua_pop(l, 3);
    lua_pushboolean(l, 0);

    return 1;
  }

  lua_pop(l, 3);
  lua_pushboolean(l, 1);

  return 1;
}

/**
 * API function which submits an event to a players narrator.
 *
 * Parameters
 *   l - The current Lua state
 *
 * Returns 0 on success or calls LuaL_Error on failure
 **/
static int lua_narrate(lua_State* l) {
  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, -2, LUA_TTABLE);

  player_t* player = lua_to_player(l, -2);
  event_t* event = lua_touserdata(l, -1);
  lua_pop(l, 2);

  game_t* game = lua_common_get_game(l);

  if (player_narrate(player, game, event) != 0) {
    return luaL_error(l, "Unable to narrate to player");
  }

  return 0;
}

/**
 * API function which retrieves the entity of a player
 *
 * l - Current Lua state
 *
 * Returns 0 on success or calls luaL_error on failure
 **/
static int lua_get_entity(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  player_t* player = lua_to_player(l, -1);
  lua_pop(l, 1);

  if (player->entity == NULL) {
    lua_pushnil(l);
  } else {
    lua_push_entity(l, player->entity);
  }

  return 1;
}

/**
 * API function which sets the entity of a player given the ID of an entity
 *
 * l - Current Lua state
 *
 * Returns 0 on success or calls LuaL_error on failure
 **/
static int lua_set_entity(lua_State* l) {
  luaL_checktype(l, -2, LUA_TTABLE);
  player_t* player = lua_to_player(l, -2);

  luaL_checktype(l, -1, LUA_TTABLE);
  entity_t* entity = lua_to_entity(l, -1);

  lua_pop(l, 2);

  player->entity = entity;

  return 0;
}

/**
 * API method which allows the currently active state of a player to be changed.
 *
 * Parameters
 *   l - The current Lua state
 *
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_set_state(lua_State* l) {
  luaL_checktype(l, -1, LUA_TUSERDATA);
  luaL_checktype(l, -2, LUA_TTABLE);

  player_t* player = lua_to_player(l, -2);
  lua_ref_t* state = lua_touserdata(l, -1);
  lua_pop(l, 2);

  game_t* game = lua_common_get_game(l);

  if (player_change_state(player, game, state) == -1) {
    return luaL_error(l, "Failed to change player state");
  }

  return 0;
}

/**
 * API method which allows the narrator of a player to be changed.
 *
 * Parameters
 *   l - The current Lua state
 *
 * Returns 0 on success or luaL_error on failure.
 **/
static int lua_set_narrator(lua_State* l) {
  luaL_checktype(l, -1, LUA_TUSERDATA);
  luaL_checktype(l, -2, LUA_TTABLE);

  lua_ref_t* narrator = lua_touserdata(l, -1);
  player_t* player = lua_to_player(l, -2);

  lua_pop(l, 2);

  player->narrator = narrator;

  return 0;
}

/**
 * API method to retrieve entities associated with a player.
 *
 * l - current Lua state
 *
 * Returns 0 on success or luaL_error on failure.
 **/
static int lua_get_entities(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  player_t* player = lua_to_player(l, -1);
  lua_pop(l, 1);

  game_t* game = lua_common_get_game(l);

  linked_list_t* results = create_linked_list_t();
  results->deallocator = deallocate;

  if (db_entity_get_ids_by_user(game->database, uuid_str(&player->user_uuid), results) == -1) {
    LOG(ERROR, "Error retrieving entity ids for player [%s]", uuid_str(&player->uuid));

    free_linked_list_t(results);
  }

  lua_newtable(l); // -1 = table

  it_t it = list_begin(results);

  char* uuid = NULL;
  int count = 1;

  while ((uuid = it_get(it)) != NULL) {
    entity_t* entity = ecs_get_entity(game, uuid);

    lua_pushnumber(l, count); // -1 = count (index), -2 = table
    lua_push_entity(l, entity); // -1 = uuid, -2 = count (index), -3 = table
    lua_rawset(l, -3); // = -1 table

    it = it_next(it);
    count++;
  }

  free_linked_list_t(results);

  return 1;
}

/**
 * API method to send text to a player.
 *
 * Parameters
 *   l - The current Lua state.
 *
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_send_to_player(lua_State* l) {
  luaL_checktype(l, -1, LUA_TSTRING);
  luaL_checktype(l, -2, LUA_TTABLE);

  player_t* player = lua_to_player(l, -2);

  const char* msg = lua_tostring(l, -1);

  lua_pop(l, 2);

  send_to_player(player, msg);

  return 0;
}

/**
 * API method to disconnect a player from the engine.
 *
 * Parameters
 *   l - The current Lua state.
 *
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_disconnect(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  player_t* player = lua_to_player(l, -1);
  lua_pop(l, 1);

  player->client->hungup = 1;

  return 0;
}

/**
 * API method to request the player disable their echo.
 *
 * l - The current Lua state
 *
 * player.disable_echo(p)
 * 
 * Returns 0 on success or calls luaL_error on failure
**/
static int lua_disable_echo(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);

  player_t* player = lua_to_player(l, -1);

  player_request_disable_echo(player);

  return 0;
}

/**
 * API method to request the player enable their echo.
 *
 * l - The current Lua state
 *
 * player.enable_echo(p)
 *
 * Returns 0 on success or calls luaL_error on failure
**/
static int lua_enable_echo(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);

  player_t* player = lua_to_player(l, -1);

  player_request_enable_echo(player);

  return 0;
}

/**
 * API method to send a GMCP message to a player.
 * l - The current Lua state
 *
 * player.send_gmcp(p, "topic", "msg")
 *
 * Returns 0 or calls luaL_error on error
**/
static int lua_send_gmcp(lua_State* l) {
  char* msg = NULL;
  char* topic = NULL;
  player_t* player = NULL;

  luaL_checktype(l, -1, LUA_TSTRING);

  if (lua_type(l, -2) == LUA_TSTRING) {
    luaL_checktype(l, -3, LUA_TTABLE);

    msg = strdup(lua_tostring(l, -1));
    topic = strdup(lua_tostring(l, -2));
    player = lua_to_player(l, -3);

    lua_pop(l, 3);
  } else if (lua_type(l, -2) == LUA_TTABLE) {
    topic = strdup(lua_tostring(l, -1));
    player = lua_to_player(l, -2);

    lua_pop(l, 2);
  } else {
    return luaL_error(l, "Invalid parameters to send_gmcp");
  }  

  send_gmcp_to_player(player, topic, msg);

  free(topic);

  if (msg != NULL) {
    free(msg);
  }
  
  return 0;
}
