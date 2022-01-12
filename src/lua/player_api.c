#include "lua.h"
#include "lauxlib.h"

#include "mud/lua/common.h"
#include "mud/lua/player_api.h"
#include "mud/network/client.h"
#include "mud/player.h"

static int lua_send_to_player(lua_State *l);
static int lua_disconnect(lua_State *l);

static const struct luaL_Reg player_lib [] = {
  {"send", lua_send_to_player},
  {"disconnect", lua_disconnect},
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
  lua_common_assert_n_arguments(l, 2);
  
  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  player_t* player = lua_touserdata(l, 1);
  const char* msg = luaL_checkstring(l, 2);
  lua_pop(l, 2);

  send_to_player(player, msg);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_disconnect(lua_State *l) {
  lua_common_assert_n_arguments(l, 1);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  player_t* player = lua_touserdata(l, 1);
  
  player->client->hungup = 1;

  lua_pop(l, 1);

  return 0;
}
