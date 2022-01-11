#include "lua.h"
#include "lauxlib.h"

#include "mud/lua/log_api.h"
#include "mud/log.h"

#define MAX_SCRIPT_INFO_LINE_LENGTH 128

static lua_Debug get_debug_info(lua_State *l);

static int lua_log_trace(lua_State *l);
static int lua_log_debug(lua_State *l);
static int lua_log_info(lua_State *l);
static int lua_log_warn(lua_State *l);
static int lua_log_error(lua_State *l);

static const struct luaL_Reg log_lib [] = {
  {"trace", lua_log_trace},
  {"debug", lua_log_debug},
  {"info", lua_log_info},
  {"warn", lua_log_warn},
  {"error", lua_log_error},
  {NULL, NULL}
};

/**
 * TODO(Chris I)
**/
int lua_log_register_api(lua_State* l) {
  luaL_newlib(l, log_lib);
  lua_setglobal(l, "log");

  return 0;
}

/**
 * TODO(Chris I)
**/
static lua_Debug get_debug_info(lua_State *l) {
  lua_Debug debug;
  lua_getstack(l, 1, &debug);
  lua_getinfo(l, "nSl", &debug);

  return debug;
}

/**
 * TODO(Chris I)
**/
static int lua_log_trace(lua_State *l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_log_trace(): Expected 1 argument");
    lua_error(l);
  }

  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(TRACE, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_log_debug(lua_State *l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_log_debug(): Expected 1 argument");
    lua_error(l);
  }

  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(DEBUG, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_log_info(lua_State *l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_log_info(): Expected 1 argument");
    lua_error(l);
  }

  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(INFO, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_log_warn(lua_State *l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_log_warn(): Expected 1 argument");
    lua_error(l);
  }

  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(WARN, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_log_error(lua_State *l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_log_error(): Expected 1 argument");
    lua_error(l);
  }

  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(ERROR, debug.short_src, debug.currentline, message);

  return 0;
}
