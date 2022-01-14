#include <assert.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/player_api.h"
#include "mud/network/client.h"
#include "mud/player.h"
#include "mud/util/muduuid.h"

#define PLAYER_DATA_TABLE_NAME "_players"

static int lua_authenticate(lua_State* l);
static int lua_get_player_data(lua_State* l);
static int lua_save_player_data(lua_State* l);
static int lua_change_state(lua_State* l);
static int lua_send_to_player(lua_State* l);
static int lua_disconnect(lua_State* l);

static const struct luaL_Reg player_lib[] = {
  { "authenticate", lua_authenticate },
  { "get_data", lua_get_player_data },
  { "save_data", lua_save_player_data },
  { "change_state", lua_change_state },
  { "send", lua_send_to_player },
  { "disconnect", lua_disconnect },
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
 * API function which copies a table for player specific data from the master Lua state into the
 * current Lua state.  We can't keep persistent data in script Lua states as they can be unloaded.
 *
 * Parameters
 *   l - The current Lua state
 *
 * Returns 0 on success, calls luaL_error on failure.
**/
static int lua_get_player_data(lua_State* l) {
  lua_common_assert_n_arguments(l, 1);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  player_t* player = lua_touserdata(l, 1);
  lua_pop(l, 1);

  lua_State* master = lua_common_get_master_state(l);

  if (lua_getglobal(master, PLAYER_DATA_TABLE_NAME) != LUA_TTABLE) {
    return luaL_error(master, "Unable to obtain global player table [%s]", PLAYER_DATA_TABLE_NAME);
  }

  const char* player_uuid = uuid_str(&player->uuid);

  // stack 1 = global player data table
  lua_pushstring(master, player_uuid);

  // stack 1 = global player data, 2 = player uuid
  if (lua_rawget(master, 1) == LUA_TNIL) {

    // stack 1 = player data table 2 = nil
    lua_pop(master, 1);

    // stack 1 = player data table
    lua_pushstring(master, player_uuid);

    // stack 1 = player data table, 2 = player uuid
    lua_newtable(master);

    // stack 1 = player data table, 2 = player uuid, 3 = individual player data table
    lua_rawset(master, 1);

    // stack 1 = player data table
    lua_pushstring(master, player_uuid);

    // stack 1 = player data table 2 = player uuid
    if (lua_rawget(master, 1) != LUA_TTABLE) {
      return luaL_error(master, "Unable to create individual player data table for [%s]", player_uuid);
    }
  }

  // stack 1 = player data table, 2 = individual player data table
  lua_remove(master, 1);

  // stack 1 = individual player table
  lua_common_copy_table(master, l);

  lua_pop(master, 1);

  return 1;
}

/**
 * API function which copies player specific data from the current state to the master state.
 * This allows us to persist data over script reloads/unloads.
 *
 * Parameters
 *   l - The current Lua state
 *
 * Returns 0 on success or calls luaL_error on failure.
**/
static int lua_save_player_data(lua_State* l) {
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 2, LUA_TTABLE);

  player_t* player = lua_touserdata(l, 1);
  lua_State* master = lua_common_get_master_state(l);

  // l stack = 1 table, master stack empty
  if (lua_getglobal(master, PLAYER_DATA_TABLE_NAME) != LUA_TTABLE) {
    return luaL_error(master, "Unable to obtain global player table [%s]", PLAYER_DATA_TABLE_NAME);
  }

  const char* player_uuid = uuid_str(&player->uuid);

  // l stack = 1 table, master stack = 1 global player table
  lua_pushstring(master, player_uuid);

  // l stack = 1 table, master stack = 1 global player table 2 player uuid
  lua_common_copy_table(l, master);

  // l stack = 1 table, master stack = 1 global player table 2 player uuid 3 copied table
  lua_rawset(master, 1);

  lua_settop(l, 0);
  lua_settop(master, 0);

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
static int lua_change_state(lua_State* l) {
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  player_t* player = lua_touserdata(l, 1);

  const char* name = luaL_checkstring(l, 2);

  game_t* game = lua_common_get_game(l);

  if (player_change_state(player, game, name) == -1) {
    return luaL_error(l, "Failed to change player state");
  }

  lua_settop(l, 0);

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
