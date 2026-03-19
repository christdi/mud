#include "lauxlib.h"
#include "lua.h"

#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/log_api.h"

#define LOG_LIB_NAME "log"
#define MAX_SCRIPT_INFO_LINE_LENGTH 128

static lua_Debug get_debug_info(lua_State* lua);

static int lua_log_trace(lua_State* lua);
static int lua_log_debug(lua_State* lua);
static int lua_log_info(lua_State* lua);
static int lua_log_warn(lua_State* lua);
static int lua_log_error(lua_State* lua);

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
 * lua - The Lua state.
 * 
 * Returns 0 on success
 **/
int lua_log_register_api(lua_State* lua) {
  lua_push_api_table(lua);
  
  lua_pushstring(lua, LOG_LIB_NAME);
  luaL_newlib(lua, log_lib);
  
  lua_rawset(lua, -3);

  return 0;

  return 0;
}

/**
 * Lua API method to retrieve a debug structure.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static lua_Debug get_debug_info(lua_State* lua) {
  lua_Debug debug;
  lua_getstack(lua, 1, &debug);
  lua_getinfo(lua, "nSl", &debug);

  return debug;
}

/**
 * Lua API method to log a trace message.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_trace(lua_State* lua) {
  const char* message = luaL_checkstring(lua, 1);
  lua_settop(lua, 0);

  lua_Debug debug = get_debug_info(lua);
  mlog(TRACE, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log a debug message.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_debug(lua_State* lua) {
  const char* message = luaL_checkstring(lua, 1);
  lua_settop(lua, 0);

  lua_Debug debug = get_debug_info(lua);
  mlog(DEBUG, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log an info message.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_info(lua_State* lua) {
  const char* message = luaL_checkstring(lua, 1);
  lua_settop(lua, 0);

  lua_Debug debug = get_debug_info(lua);
  mlog(INFO, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log a warn message.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_warn(lua_State* lua) {
  const char* message = luaL_checkstring(lua, 1);
  lua_settop(lua, 0);

  lua_Debug debug = get_debug_info(lua);
  mlog(WARN, debug.short_src, debug.currentline, message);

  return 0;
}

/**
 * Lua API method to log an error message.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success or calls luaL_error on failure.
 **/
static int lua_log_error(lua_State* lua) {
  const char* message = luaL_checkstring(lua, 1);
  lua_settop(lua, 0);

  lua_Debug debug = get_debug_info(lua);
  mlog(ERROR, debug.short_src, debug.currentline, message);

  return 0;
}
