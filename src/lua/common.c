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
