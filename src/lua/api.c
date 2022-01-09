#include "lua.h"
#include "lauxlib.h"

#include "mud/lua/api.h"
#include "mud/log.h"

static const struct luaL_Reg mud_lib [] = {
  {"register_component", lua_register_component},
  {"has_component", lua_has_component},
  {"add_component", lua_add_component},
  {"get_component", lua_get_component},  
  {NULL, NULL}
};

int lua_register_api(lua_State* l) {
  luaL_newlib(l, mud_lib);
  lua_setglobal(l, "mud");

  return 0;
}

static int lua_register_component(lua_State* l) {
  mlog(INFO, "lua_register_component", "lua_register_component called");

  return 0;
}

static int lua_has_component(lua_State* l) {
  mlog(INFO, "lua_has_component", "lua_has_component called");

  return 0;
}

static int lua_add_component(lua_State* l) {
  mlog(INFO, "lua_add_componentt", "lua_add_component called");

  return 0;
}

static int lua_get_component(lua_State* l) {
  mlog(INFO, "lua_get_component", "lua_get_component called");

  return 0;
}

