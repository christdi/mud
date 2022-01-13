#include <assert.h>

#include "lua.h"
#include "lauxlib.h"

#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"

#define GLOBAL_GAME_FIELD_NAME "gptr"
#define GLOBAL_DB_FIELD_NAME "dbptr"
#define GLOBAL_LUA_FIELD_NAME "luaptr"
#define MAX_ERROR_LINE_LENGTH 128

/**
 * Initialises a new Lua state with global fields used for API calls.
 * 
 * Parameters
 *   l - the Lua state to be populated
 *   game - game struct containing data we want to expose
 * 
 * Returns 0 on success.
**/
int lua_common_initialise_state(lua_State *l, game_t* game) {
  assert(l);
  assert(game);

  lua_pushlightuserdata(l, game);
  lua_setglobal(l, GLOBAL_GAME_FIELD_NAME);

  lua_pushlightuserdata(l, game->database);
  lua_setglobal(l, GLOBAL_DB_FIELD_NAME);

  lua_pushlightuserdata(l, game->lua_state);
  lua_setglobal(l, GLOBAL_LUA_FIELD_NAME);

  return 0;    
}

/**
 * Intended to create a table for player data in the master Lua state.  Transient
 * script states will be able to obtain a local copy which they may then persist
 * back to the master Lua state.
 * 
 * Parameters
 *   l - The Lua state to be populated, should be master state
 * 
 * Returns 0 on success
**/
int lua_common_create_player_table(lua_State *l) {
  assert(l);

  lua_newtable(l);
  lua_setglobal(l, PLAYER_DATA_TABLE_NAME);

  return 0;
}

/**
 * Parameters
 *   l - Lua state which is currently active
 * 
 * Returns a pointer to the game struct. 
**/
game_t* lua_common_get_game(lua_State* l) {
  assert(l);

  lua_getglobal(l, GLOBAL_GAME_FIELD_NAME);
  int top = lua_gettop(l);

  luaL_checktype(l, top, LUA_TLIGHTUSERDATA);
  game_t* game = lua_touserdata(l, top);

  lua_pop(l, 1);

  return game;
}

/**
 * Parameters
 *   l - Lua state which is currently active
 * 
 * Returns a pointer to the sqlite3 database. 
**/
sqlite3* lua_common_get_database(lua_State* l) {
  assert(l);

  lua_getglobal(l, GLOBAL_DB_FIELD_NAME);
  int top = lua_gettop(l);

  luaL_checktype(l, top, LUA_TLIGHTUSERDATA);
  sqlite3* db = lua_touserdata(l, top);
  
  lua_pop(l, 1);

  return db;  
}

/**
 * Parameters
 *   l - Lua state which is currently active
 * 
 * Returns a pointer to the master Lua state. 
**/
lua_State* lua_common_get_master_state(lua_State* l) {
  assert(l);

  lua_getglobal(l, GLOBAL_LUA_FIELD_NAME);
  int top = lua_gettop(l);

  luaL_checktype(l, top, LUA_TLIGHTUSERDATA);
  lua_State* master_state = lua_touserdata(l, top);

  lua_pop(l, 1);

  return master_state;
}

/**
 * Configures a Lua debug struct with script/line information, mostly for logging.
 * 
 * Parameters
 *   l - Lua state which is currently active
 * 
 * Returns a copy of the Lua debug struct
**/
lua_Debug lua_common_get_debug(lua_State* l) {
  assert(l);

  lua_Debug debug;
  lua_getstack(l, 1, &debug);
  lua_getinfo(l, "nSl", &debug);

  return debug;
}

/**
 * Asserts the amount of elements on the Lua stack, intended to be used to determine
 * an API function has received the expected amount of parameters.
 * 
 * Parameters
 *   l - Lua state which is currently active
 *   n - the amount of expected elements on the stack
 * 
 * Returns 0 on success or a Lua error on failure.
**/
int lua_common_assert_n_arguments(lua_State *l, int n) {
  assert(l);

  int count = lua_gettop(l);

  if (count != n) {
    return luaL_error(l, "Expected %d arguments but received %d", n, count);
  }

  return 0;
}

/**
 * Copies the table from the top of the stack of origin and pushes it to the top of the stack
 * of dest.
 * 
 * Parameters
 *   origin - The origin Lua state
 *   dest - The destination Lua state
 * 
 * Returns 0 on success or throws a Lua error
**/
int lua_common_copy_table(lua_State* origin, lua_State* dest) {
  luaL_checktype(origin, -1, LUA_TTABLE);

  // origin stack = 1 table, dest stack = empty
  lua_pushnil(origin);
  lua_newtable(dest);

  // origin stack = 1/-2 table 2/-1 = key (nil initially), dest stack = 1/-1 new table
  while (lua_next(origin, -2) != 0) {
    // origin stack = 1/-3 table 2/-2 key 3/-1 value, dest stack = 1/-1 new table
    if (lua_type(origin, -2) == LUA_TNUMBER) {
      int key = lua_tonumber(origin, -2);
      LOG(INFO, "Key is: [%d]", key);
      lua_pushnumber(dest, key);
    } else if (lua_type(origin, -2) == LUA_TSTRING) {
      const char* key = lua_tostring(origin, -2);
      LOG(INFO, "Key is: [%s]", key);
      lua_pushstring(dest, key);
    } else {
      return luaL_error(origin, "Unsupported key type, must be number or string");
    }

    // origin stack = 1/-3 table 2/-2 key 3/-1 value, dest stack = 1/-2 new table 2/-1 key
    if (lua_type(origin, -1) == LUA_TNUMBER) {
      int value = lua_tonumber(origin, -1);
      lua_pushnumber(dest, value);
    } else if (lua_type(origin, -1) == LUA_TSTRING) {
      const char* value = lua_tostring(origin, -1);
      lua_pushstring(dest, value);
    } else {
      return luaL_error(origin, "Unsupported value type, must be number or string");
    }

    // origin stack = 1/-3 table 2/-2 key 3/-1 value, dest stack 1/-3 = new table 2/-2 key 3/-1 value
    lua_rawset(dest, -3);

    // origin stack = 1/-3 table 2/-2 key 3/-3 value, dest atack = 1/-1 new table
    lua_pop(origin, 1);
  }

  return 0;
}
