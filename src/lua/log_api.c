#include "lauxlib.h"
#include "lua.h"

#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/log_api.h"

#define LOG_LIB_NAME "log"
#define MAX_SCRIPT_INFO_LINE_LENGTH 128

static lua_Debug get_debug_info(lua_State* l);

static int lua_log_trace(lua_State* l);
static int lua_log_debug(lua_State* l);
static int lua_log_info(lua_State* l);
static int lua_log_warn(lua_State* l);
static int lua_log_error(lua_State* l);

static const struct luaL_Reg log_lib[] = {
  { "trace", lua_log_trace },
  { "debug", lua_log_debug },
  { "info", lua_log_info },
  { "warn", lua_log_warn },
  { "error", lua_log_error },
  { NULL, NULL }
};

/**
 * Registers the log module with the Lua state.
 * 
 * l - The Lua state.
 * 
 * Returns 0 on success
 **/
int lua_log_register_api(lua_State* l) {
  lua_push_api_table(l);
  
  lua_pushstring(l, LOG_LIB_NAME);
  luaL_newlib(l, log_lib);
  
  lua_rawset(l, -3);

  return 0;

  return 0;
}

/**
 * Lua API method to retrieve a debug structure.
 * 
 * l - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static lua_Debug get_debug_info(lua_State* l) {
  lua_Debug debug;
  lua_getstack(l, 1, &debug);
  lua_getinfo(l, "nSl", &debug);

  return debug;
}

/**
 * Lua API method to log a trace message.
 * 
 * l - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_trace(lua_State* l) {
  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(TRACE, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log a debug message.
 * 
 * l - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_debug(lua_State* l) {
  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(DEBUG, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log an info message.
 * 
 * l - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_info(lua_State* l) {
  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(INFO, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log a warn message.
 * 
 * l - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_warn(lua_State* l) {
  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(WARN, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log an error message.
 * 
 * l - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_error(lua_State* l) {
  const char* message = luaL_checkstring(l, 1);
  lua_settop(l, 0);

  lua_Debug debug = get_debug_info(l);
  mlog(ERROR, debug.short_src, debug.currentline, message);

  return 0;
}
