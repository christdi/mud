#include <assert.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/event/event.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/player_api.h"
#include "mud/narrator.h"
#include "mud/network/client.h"
#include "mud/player.h"
#include "mud/util/muduuid.h"

#define PLAYER_DATA_TABLE_NAME "_players"

static int lua_authenticate(lua_State* l);
static int lua_narrate(lua_State* l);
static int lua_set_state(lua_State* l);
static int lua_set_narrator(lua_State* l);
static int lua_send_to_player(lua_State* l);
static int lua_disconnect(lua_State* l);
static int lua_uuid(lua_State* l);

static const struct luaL_Reg player_lib[] = {
  { "authenticate", lua_authenticate },
  { "narrate", lua_narrate },
  { "set_state", lua_set_state },
  { "set_narrator", lua_set_narrator },
  { "send", lua_send_to_player },
  { "disconnect", lua_disconnect },
  { "uuid", lua_uuid },
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
  lua_common_assert_n_arguments(l, 3);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  player_t* player = lua_touserdata(l, 1);
  const char* username = luaL_checkstring(l, 2);
  const char* password = luaL_checkstring(l, 3);

  game_t* game = lua_common_get_game(l);

  if (player_authenticate(player, game, username, password) == -1) {
    lua_settop(l, 0);
    lua_pushboolean(l, 0);

    return 1;
  }

  lua_settop(l, 0);
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
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 2, LUA_TLIGHTUSERDATA);

  player_t* player = lua_touserdata(l, 1);
  event_t* event = lua_touserdata(l, 2);
  game_t* game = lua_common_get_game(l);

  lua_settop(l, 0);

  if (player_narrate(player, game, event) != 0) {
    return luaL_error(l, "Unable to narrate to player");
  }

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
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 2, LUA_TUSERDATA);

  player_t* player = lua_touserdata(l, 1);
  state_t* state = lua_touserdata(l, 2);
  game_t* game = lua_common_get_game(l);

  lua_settop(l, 0);

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
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 2, LUA_TUSERDATA);

  player_t* player = lua_touserdata(l, 1);
  narrator_t* narrator = lua_touserdata(l, 2);

  lua_settop(l, 0);

  player->narrator = narrator;

  return 0;
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
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  player_t* player = lua_touserdata(l, 1);
  const char* msg = luaL_checkstring(l, 2);
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
  lua_common_assert_n_arguments(l, 1);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  player_t* player = lua_touserdata(l, 1);

  player->client->hungup = 1;

  lua_pop(l, 1);

  return 0;
}

/**
 * API method to retrieve the UUID of a player.
 *
 * Parameters
 *   l - The current Lua state.
 *
 * Returns 0 on success or calls luaL_error on failure. 
**/
static int lua_uuid(lua_State* l) {
  lua_common_assert_n_arguments(l, 1);
  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  player_t* player = lua_touserdata(l, 1);
  lua_settop(l, 0);

  lua_pushstring(l, uuid_str(&player->uuid));

  return 1;
}
