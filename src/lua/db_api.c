#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"

#include "mud/lua/db_api.h"
#include "mud/log.h"

static sqlite3* get_database_global(lua_State *l);

static int lua_db_prepare_statement(lua_State *l);
static int lua_db_bind(lua_State *l);
static int lua_db_step(lua_State* l);
static int lua_db_column_text(lua_State* l);
static int lua_db_finalize(lua_State* l);

static const struct luaL_Reg db_lib [] = {
  {"prepare", lua_db_prepare_statement},
  {"bind", lua_db_bind},
  {"step", lua_db_step},
  {"column_text", lua_db_column_text},
  {"finalize", lua_db_finalize},
  {NULL, NULL}
};

/**
 * TODO(Chris I)
**/
static sqlite3* get_database_global(lua_State *l) {
  lua_getglobal(l, "database");

  int top = lua_gettop(l);

  if (lua_islightuserdata(l, top) != 1) {
    LOG(ERROR, "Could not retrieve sqlite3 pointer");

    return NULL;
  }

  sqlite3* db = lua_touserdata(l, top);
  lua_pop(l, 1);

  return db;
}

/**
 * TODO(Chris I)
**/
int lua_db_register_api(lua_State* l, sqlite3* db) {
  luaL_newlib(l, db_lib);
  lua_setglobal(l, "db");

  lua_pushlightuserdata(l, db);
  lua_setglobal(l, "database");

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_db_prepare_statement(lua_State *l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_db_prepare_statement(): Expected one argument");
    lua_error(l);
  }

  sqlite3* db = NULL;

  if ((db = get_database_global(l)) == NULL) {
    lua_pushliteral(l, "lua_db_prepare_statement(): Unable to retrieve sqlite3 pointer");
    lua_error(l);

    return 0;
  }

  const char* sql = luaL_checkstring(l, -1);

  sqlite3_stmt* res = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    lua_pushstring(l, sqlite3_errmsg(db));
    lua_error(l);
  }

  lua_pushlightuserdata(l, res);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_db_bind(lua_State *l) {
  sqlite3* db = NULL;

  if ((db = get_database_global(l)) == NULL) {
    lua_pushliteral(l, "lua_db_prepare_statement(): Unable to retrieve sqlite3 pointer");
    lua_error(l);

    return 0;
  }

  if (lua_gettop(l) != 3) {
    lua_pushliteral(l, "lua_db_bind(): Expected three arguments");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_db_bind(): First argument should be C userdata pointer to prepared statement but was not");
    lua_error(l);
  }

  sqlite3_stmt* res = lua_touserdata(l, 1);
  int field = luaL_checknumber(l, 2);

  if (lua_type(l, 3) == LUA_TSTRING) {
    const char* value = lua_tostring(l, 3);

    if (sqlite3_bind_text(res, field, value, (int)strlen(value), NULL) != SQLITE_OK) {
      sqlite3_finalize(res);

      lua_pushstring(l, sqlite3_errmsg(db));
      lua_error(l);
    }
  }

  if (lua_type(l, 3) == LUA_TNUMBER) {
    int value = lua_tonumber(l, 3);

    if (sqlite3_bind_int(res, field, value) != SQLITE_OK) {
      sqlite3_finalize(res);

      lua_pushstring(l, sqlite3_errmsg(db));
      lua_error(l);
    }
  }

  lua_pop(l, 3);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_db_step(lua_State* l) {
  sqlite3* db = NULL;

  if ((db = get_database_global(l)) == NULL) {
    lua_pushliteral(l, "lua_db_prepare_statement(): Unable to retrieve sqlite3 pointer");
    lua_error(l);

    return 0;
  }

  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_db_step(): Expected one argument");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_db_step(): First argument should be C userdata pointer to prepared statement but was not");
    lua_error(l);
  }

  sqlite3_stmt* res = lua_touserdata(l, 1);
  lua_pop(l, 1);

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      lua_pushboolean(l, 0);

      return 1;
    }

    sqlite3_finalize(res);

    lua_pushstring(l, sqlite3_errmsg(db));
    lua_error(l);
  }

  lua_pushboolean(l, 1);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_db_column_text(lua_State* l) {
  if (lua_gettop(l) != 2) {
    lua_pushliteral(l, "lua_db_column_text(): Expected two arguments");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_db_column_text(): First argument should be C userdata pointer to prepared statement but was not");
    lua_error(l);
  }

  sqlite3_stmt* res = lua_touserdata(l, 1);
  int row = luaL_checknumber(l, 2);

  lua_pop(l, 2);

  lua_pushstring(l, (char *)sqlite3_column_text(res, row));

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_db_finalize(lua_State* l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_db_finalize(): Expected one arguments");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_db_column_text(): First argument should be C userdata pointer to prepared statement but was not");
    lua_error(l);
  }

  sqlite3_stmt* res = lua_touserdata(l, 1);

  sqlite3_finalize(res);

  return 0;
}
