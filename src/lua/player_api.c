#include "lua.h"
#include "lauxlib.h"

#include "mud/lua/player_api.h"
#include "mud/player.h"

static int lua_send_to_player(lua_State *l);

static const struct luaL_Reg player_lib [] = {
  {"send", lua_send_to_player},
  {NULL, NULL}
};

/**
 * TODO(Chris I)
**/
int lua_player_register_api(lua_State* l) {
  luaL_newlib(l, player_lib);
  lua_setglobal(l, "player");

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_send_to_player(lua_State *l) {
  if (lua_gettop(l) != 2) {
    lua_pushliteral(l, "lua_send_to_player(): Expected 2 arguments");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_send_to_player(): Expected first argument to be pointer to player");
    lua_error(l);
  }

  player_t* player = lua_touserdata(l, 1);
  const char* msg = luaL_checkstring(l, 2);
  lua_pop(l, 2);

  send_to_player(player, msg);

  return 0;
}
