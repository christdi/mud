#include <assert.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"

#define GLOBAL_GAME_FIELD_NAME "gptr"
#define GLOBAL_DB_FIELD_NAME "dbptr"
#define GLOBAL_C_NULL_FIELD_NAME "cnull"
#define GLOBAL_API_TABLE_NAME "lunac"
#define GLOBAL_API_FIELD_NAME "api"
#define MAX_ERROR_LINE_LENGTH 128
#define LOG_STACK_TYPE_SIZE 128

/**
 * Initialises a new Lua state with global fields used for API calls.
 *
 * Parameters
 *   l - the Lua state to be populated
 *   game - game struct containing data we want to expose
 *
 * Returns 0 on success.
 **/
int lua_initialise_state(lua_State* l, game_t* game) {
  assert(l);
  assert(game);

  lua_pushlightuserdata(l, game);
  lua_setglobal(l, GLOBAL_GAME_FIELD_NAME);

  lua_pushlightuserdata(l, game->database);
  lua_setglobal(l, GLOBAL_DB_FIELD_NAME);

  lua_pushlightuserdata(l, NULL);
  lua_setglobal(l, GLOBAL_C_NULL_FIELD_NAME);

  lua_newtable(l);
  lua_pushstring(l, GLOBAL_API_FIELD_NAME);
  lua_newtable(l);
  lua_rawset(l, -3);

  lua_setglobal(l, GLOBAL_API_TABLE_NAME);

  return 0;
}

/**
 * Pushes the API table onto the stack.
 * 
 * l - The calling Lua state.
**/
void lua_push_api_table(lua_State* l) {
  lua_getglobal(l, GLOBAL_API_TABLE_NAME);
  lua_pushstring(l, GLOBAL_API_FIELD_NAME);
  lua_rawget(l, -2);
  lua_remove(l, -2);
}

/**
 * Parameters
 *   l - Lua state which is currently active
 *
 * Returns a pointer to the game struct.
 **/
game_t* lua_get_game(lua_State* l) {
  assert(l);

  lua_getglobal(l, GLOBAL_GAME_FIELD_NAME);

  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  game_t* game = lua_touserdata(l, -1);

  lua_pop(l, 1);

  return game;
}

/**
 * Parameters
 *   l - Lua state which is currently active
 *
 * Returns a pointer to the sqlite3 database.
 **/
sqlite3* lua_get_database(lua_State* l) {
  assert(l);

  lua_getglobal(l, GLOBAL_DB_FIELD_NAME);
  int top = lua_gettop(l);

  luaL_checktype(l, top, LUA_TLIGHTUSERDATA);
  sqlite3* db = lua_touserdata(l, top);

  lua_pop(l, 1);

  return db;
}

/**
 * Configures a Lua debug struct with script/line information, mostly for logging.
 *
 * Parameters
 *   l - Lua state which is currently active
 *
 * Returns a copy of the Lua debug struct
 **/
lua_Debug lua_get_debug(lua_State* l) {
  assert(l);

  lua_Debug debug;
  lua_getstack(l, 1, &debug);
  lua_getinfo(l, "nSl", &debug);

  return debug;
}

/**
 * Outputs the contents a Lua state stack to logging.
 *
 * Parameters
 *   l - The lua state whose stack should be printed
 **/
void lua_log_stack(lua_State* l) {
  int top = lua_gettop(l);

  int i = 1;

  LOG(DEBUG, "LUA STACK");

  for (i = 1; i <= top; i++) {
    char details[LOG_STACK_TYPE_SIZE];

    switch (lua_type(l, i)) {
    case LUA_TNUMBER:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%g", lua_tonumber(l, i));
      break;
    case LUA_TSTRING:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%s", lua_tostring(l, i));
      break;
    case LUA_TBOOLEAN:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%s", lua_toboolean(l, i) ? "true" : "false");
      break;
    case LUA_TNIL:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%s", "nil");
      break;
    default:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%p", lua_topointer(l, i));
      break;
    }

    LOG(DEBUG, "%d - %s [%s]", i, luaL_typename(l, i), details);
  }

  LOG(DEBUG, "");
}
