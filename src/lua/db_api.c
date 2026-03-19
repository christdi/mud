#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/db_api.h"

#define DB_LIB_NAME "db"

static int lua_db_prepare_statement(lua_State* lua);
static int lua_db_bind(lua_State* lua);
static int lua_db_step(lua_State* lua);
static int lua_db_column_text(lua_State* lua);
static int lua_db_finalize(lua_State* lua);

static const struct luaL_Reg db_lib[] = {
  { "prepare", lua_db_prepare_statement },
  { "bind", lua_db_bind },
  { "step", lua_db_step },
  { "column_text", lua_db_column_text },
  { "finalize", lua_db_finalize },
  { NULL, NULL }
};

/**
 * Registers the db module with the Lua state.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success
 **/
int lua_db_register_api(lua_State* lua) {
  lua_push_api_table(lua);
  
  lua_pushstring(lua, DB_LIB_NAME);
  luaL_newlib(lua, db_lib);
  
  lua_rawset(lua, -3);

  return 0;
}

/**
 * Lua API method for preparing a statement.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_db_prepare_statement(lua_State* lua) {
  sqlite3* database = lua_get_database(lua);

  const char* sql = luaL_checkstring(lua, 1);

  sqlite3_stmt* res = NULL;

  if (sqlite3_prepare_v2(database, sql, -1, &res, 0) != SQLITE_OK) {
    return luaL_error(lua, "Error preparing statement [%s]", sqlite3_errmsg(database));
  }

  lua_pop(lua, 1);

  lua_pushlightuserdata(lua, res);

  return 1;
}

/**
 * Lua API method for binding a value to a statement.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 */
static int lua_db_bind(lua_State* lua) {
  sqlite3* database = lua_get_database(lua);

  luaL_checktype(lua, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(lua, 1);
  int field = luaL_checknumber(lua, 2);

  if (lua_type(lua, 3) == LUA_TSTRING) {
    const char* value = lua_tostring(lua, 3);

    if (sqlite3_bind_text(res, field, value, (int)strlen(value), NULL) != SQLITE_OK) {
      sqlite3_finalize(res);

      return luaL_error(lua, "Error binding text [%s]", sqlite3_errmsg(database));
    }
  }

  if (lua_type(lua, 3) == LUA_TNUMBER) {
    int value = lua_tonumber(lua, 3);

    if (sqlite3_bind_int(res, field, value) != SQLITE_OK) {
      sqlite3_finalize(res);

      return luaL_error(lua, "Error binding integer [%s]", sqlite3_errmsg(database));
    }
  }

  lua_pop(lua, 3);

  return 0;
}

/**
 * Lua API method for stepping a statement.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_db_step(lua_State* lua) {
  sqlite3* database = lua_get_database(lua);

  luaL_checktype(lua, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(lua, 1);

  int ret = 0;

  if ((ret = sqlite3_step(res)) != SQLITE_ROW) {
    if (ret == SQLITE_DONE) {
      lua_pushboolean(lua, 0);

      return 1;
    }

    sqlite3_finalize(res);

    luaL_error(lua, "Error stepping through rows [%s]", sqlite3_errmsg(database));
  }

  lua_pop(lua, 1);

  lua_pushboolean(lua, 1);

  return 1;
}

/**
 * Lua API method for getting the text value of a column.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_db_column_text(lua_State* lua) {
  luaL_checktype(lua, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(lua, 1);
  int row = luaL_checknumber(lua, 2);
  lua_pop(lua, 2);

  lua_pushstring(lua, (char*)sqlite3_column_text(res, row));

  return 1;
}

/**
 * Lua API method for finalizing a statement.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success.
 **/
static int lua_db_finalize(lua_State* lua) {
  luaL_checktype(lua, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(lua, 1);

  sqlite3_finalize(res);

  lua_pop(lua, 1);

  return 0;
}
