#include <assert.h>

#include "lua.h"
#include "lauxlib.h"

#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"

#define GLOBAL_GAME_FIELD_NAME "gptr"
#define GLOBAL_DB_FIELD_NAME "dbptr"
#define MAX_ERROR_LINE_LENGTH 128

/**
 * TODO(Chris I)
**/
int lua_common_initialise_state(lua_State *l, game_t* game) {
  assert(l);
  assert(game);

  lua_pushlightuserdata(l, game);
  lua_setglobal(l, GLOBAL_GAME_FIELD_NAME);

  lua_pushlightuserdata(l, game->database);
  lua_setglobal(l, GLOBAL_DB_FIELD_NAME);

  return 0;    
}

/**
 * TODO(Chris I)
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
 * TODO(Chris I)
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
 * TODO(Chris I)
**/
lua_Debug lua_common_get_debug(lua_State* l) {
  assert(l);

  lua_Debug debug;
  lua_getstack(l, 1, &debug);
  lua_getinfo(l, "nSl", &debug);

  return debug;
}

/**
 * TODO(Chris I)
**/
int lua_common_assert_n_arguments(lua_State *l, int n) {
  assert(l);

  int count = lua_gettop(l);

  if (count != n) {
    return luaL_error(l, "Expected %d arguments but received %d", n, count);
  }

  return 0;
}
