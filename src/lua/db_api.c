#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/db_api.h"

static int lua_db_prepare_statement(lua_State* l);
static int lua_db_bind(lua_State* l);
static int lua_db_step(lua_State* l);
static int lua_db_column_text(lua_State* l);
static int lua_db_finalize(lua_State* l);

static const struct luaL_Reg db_lib[] = {
  { "prepare", lua_db_prepare_statement },
  { "bind", lua_db_bind },
  { "step", lua_db_step },
  { "column_text", lua_db_column_text },
  { "finalize", lua_db_finalize },
  { NULL, NULL }
};

/**
 * TODO(Chris I)
 **/
int lua_db_register_api(lua_State* l) {
  luaL_newlib(l, db_lib);
  lua_setglobal(l, "db");

  return 0;
}

/**
 * TODO(Chris I)
 **/
static int lua_db_prepare_statement(lua_State* l) {
  sqlite3* db = lua_get_database(l);

  const char* sql = luaL_checkstring(l, 1);

  sqlite3_stmt* res = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    return luaL_error(l, "Error preparing statement [%s]", sqlite3_errmsg(db));
  }

  lua_pop(l, 1);

  lua_pushlightuserdata(l, res);

  return 1;
}

/**
 * TODO(Chris I)
 **/
static int lua_db_bind(lua_State* l) {
  sqlite3* db = lua_get_database(l);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(l, 1);
  int field = luaL_checknumber(l, 2);

  if (lua_type(l, 3) == LUA_TSTRING) {
    const char* value = lua_tostring(l, 3);

    if (sqlite3_bind_text(res, field, value, (int)strlen(value), NULL) != SQLITE_OK) {
      sqlite3_finalize(res);

      return luaL_error(l, "Error binding text [%s]", sqlite3_errmsg(db));
    }
  }

  if (lua_type(l, 3) == LUA_TNUMBER) {
    int value = lua_tonumber(l, 3);

    if (sqlite3_bind_int(res, field, value) != SQLITE_OK) {
      sqlite3_finalize(res);

      return luaL_error(l, "Error binding integer [%s]", sqlite3_errmsg(db));
    }
  }

  lua_pop(l, 3);

  return 0;
}

/**
 * TODO(Chris I)
 **/
static int lua_db_step(lua_State* l) {
  sqlite3* db = lua_get_database(l);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(l, 1);

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      lua_pushboolean(l, 0);

      return 1;
    }

    sqlite3_finalize(res);

    luaL_error(l, "Error stepping through rows [%s]", sqlite3_errmsg(db));
  }

  lua_pop(l, 1);

  lua_pushboolean(l, 1);

  return 1;
}

/**
 * TODO(Chris I)
 **/
static int lua_db_column_text(lua_State* l) {
  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(l, 1);
  int row = luaL_checknumber(l, 2);
  lua_pop(l, 2);

  lua_pushstring(l, (char*)sqlite3_column_text(res, row));

  return 1;
}

/**
 * TODO(Chris I)
 **/
static int lua_db_finalize(lua_State* l) {
  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  sqlite3_stmt* res = lua_touserdata(l, 1);

  sqlite3_finalize(res);

  lua_pop(l, 1);

  return 0;
}
