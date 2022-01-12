#include "lua.h"
#include "lauxlib.h"

#include "mud/lua/script_api.h"

static int lua_script_load(lua_State *l);
static int lua_script_unload(lua_State *l);
static int lua_script_reload(lua_State *l);
static int lua_script_loaded(lua_State *l);
static int lua_script_available(lua_State *l);

static const struct luaL_Reg script_lib [] = {
  {"load", lua_script_load},
  {"unload", lua_script_unload},  
  {"reload", lua_script_reload},
  {"loaded", lua_script_loaded},
  {"available", lua_script_available},  
  {NULL, NULL}
};

/**
 * TODO(Chris I)
**/
int lua_script_register_api(lua_State* l) {
  luaL_newlib(l, script_lib);
  lua_setglobal(l, "script");

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_script_load(lua_State *l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_script_load(): Expected 1 argument");
    lua_error(l);
  }

  const char* uuid = luaL_checkstring(l, 1);
  


  lua_pop(l, 1);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_script_unload(lua_State *l) {
  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_script_reload(lua_State *l) {
  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_script_loaded(lua_State *l) {
  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_script_available(lua_State *l) {
  return 0;
}
