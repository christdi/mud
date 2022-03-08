#include <assert.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/command/command.h"
#include "mud/log.h"
#include "mud/lua/command_api.h"
#include "mud/lua/common.h"
#include "mud/player.h"

static int lua_execute_command(lua_State* l);

static const struct luaL_Reg command_lib[] = {
  { "execute", lua_execute_command },

  { NULL, NULL }
};

/**
 * Registers the command API into a Lua state.
 * 
 * Parameters
 *   l - The Lua state to register the API with
 * 
 * Returns 0 on success.
**/
int lua_command_register_api(lua_State* l) {
  luaL_newlib(l, command_lib);
  lua_setglobal(l, "command");

  return 0;
}

/**
 * Attempts to execute a command for a player.
 *
 * Parameters
 *   l - The current Lua state
 *
 * Returns 0 on success or calls luaL_error on failure.
**/
static int lua_execute_command(lua_State* l) {
  lua_common_assert_n_arguments(l, 3);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  player_t* player = lua_touserdata(l, 1);

  const char* command = luaL_checkstring(l, 2);
  const char* arguments = luaL_checkstring(l, 3);

  game_t* game = lua_common_get_game(l);

  lua_settop(l, 0);

  if (execute_command(game, player, command, arguments) == -1) {
    lua_pushboolean(l, 0);

    return 1;
  }

  lua_pushboolean(l, 1);

  return 1;
}
