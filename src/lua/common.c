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
 *   lua - the Lua state to be populated
 *   game - game struct containing data we want to expose
 *
 * Returns 0 on success.
 **/
int lua_initialise_state(lua_State* lua, game_t* game) {
  assert(lua);
  assert(game);

  lua_pushlightuserdata(lua, game);
  lua_setglobal(lua, GLOBAL_GAME_FIELD_NAME);

  lua_pushlightuserdata(lua, game->database);
  lua_setglobal(lua, GLOBAL_DB_FIELD_NAME);

  lua_pushlightuserdata(lua, NULL);
  lua_setglobal(lua, GLOBAL_C_NULL_FIELD_NAME);

  lua_newtable(lua);
  lua_pushstring(lua, GLOBAL_API_FIELD_NAME);
  lua_newtable(lua);
  lua_rawset(lua, -3);

  lua_setglobal(lua, GLOBAL_API_TABLE_NAME);

  return 0;
}

/**
 * Pushes the API table onto the stack.
 * 
 * lua - The calling Lua state.
**/
void lua_push_api_table(lua_State* lua) {
  lua_getglobal(lua, GLOBAL_API_TABLE_NAME);
  lua_pushstring(lua, GLOBAL_API_FIELD_NAME);
  lua_rawget(lua, -2);
  lua_remove(lua, -2);
}

/**
 * Parameters
 *   lua - Lua state which is currently active
 *
 * Returns a pointer to the game struct.
 **/
game_t* lua_get_game(lua_State* lua) {
  assert(lua);

  lua_getglobal(lua, GLOBAL_GAME_FIELD_NAME);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  game_t* game = lua_touserdata(lua, -1);

  lua_pop(lua, 1);

  return game;
}

/**
 * Parameters
 *   lua - Lua state which is currently active
 *
 * Returns a pointer to the sqlite3 database.
 **/
sqlite3* lua_get_database(lua_State* lua) {
  assert(lua);

  lua_getglobal(lua, GLOBAL_DB_FIELD_NAME);
  int top = lua_gettop(lua);

  luaL_checktype(lua, top, LUA_TLIGHTUSERDATA);
  sqlite3* database = lua_touserdata(lua, top);

  lua_pop(lua, 1);

  return database;
}

/**
 * Configures a Lua debug struct with script/line information, mostly for logging.
 *
 * Parameters
 *   lua - Lua state which is currently active
 *
 * Returns a copy of the Lua debug struct
 **/
lua_Debug lua_get_debug(lua_State* lua) {
  assert(lua);

  lua_Debug debug;
  lua_getstack(lua, 1, &debug);
  lua_getinfo(lua, "nSl", &debug);

  return debug;
}

/**
 * Outputs the contents a Lua state stack to logging.
 *
 * Parameters
 *   lua - The lua state whose stack should be printed
 **/
void lua_log_stack(lua_State* lua) {
  int top = lua_gettop(lua);

  int idx = 1;

  LOG(DEBUG, "LUA STACK");

  for (idx = 1; idx <= top; idx++) {
    char details[LOG_STACK_TYPE_SIZE];

    switch (lua_type(lua, idx)) {
    case LUA_TNUMBER:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%g", lua_tonumber(lua, idx));
      break;
    case LUA_TSTRING:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%s", lua_tostring(lua, idx));
      break;
    case LUA_TBOOLEAN:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%s", lua_toboolean(lua, idx) ? "true" : "false");
      break;
    case LUA_TNIL:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%s", "nil");
      break;
    default:
      snprintf(details, LOG_STACK_TYPE_SIZE, "%p", lua_topointer(lua, idx));
      break;
    }

    LOG(DEBUG, "%d - %s [%s]", idx, luaL_typename(lua, idx), details);
  }

  LOG(DEBUG, "");
}
