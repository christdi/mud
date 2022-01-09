#include "lua.h"
#include "lauxlib.h"

#include "mud/game.h"
#include "mud/lua/api.h"
#include "mud/log.h"

#include <stdio.h>

static int lua_register_component(lua_State* l);
static int lua_has_component(lua_State* l);
static int lua_add_component(lua_State* l);
static int lua_get_component(lua_State* l);

static const struct luaL_Reg mud_lib [] = {
  {"register_component", lua_register_component},
  {"has_component", lua_has_component},
  {"add_component", lua_add_component},
  {"get_component", lua_get_component},  
  {NULL, NULL}
};

int lua_register_api(lua_State* l, game_t* game) {
  luaL_newlib(l, mud_lib);
  lua_setglobal(l, "mud");

  lua_pushlightuserdata(l, game);
  lua_setglobal(l, "game");

  return 0;
}

static int lua_register_component(lua_State* l) {
  mlog(INFO, "lua_register_component", "lua_register_component called");

  lua_getglobal(l, "game");

  if (lua_islightuserdata(l, -1) != 1) {
    mlog(ERROR, "lua_register_component", "Could not retrieve game pointer");

    return 0;
  }

  game_t* game = lua_touserdata(l, -1);

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

