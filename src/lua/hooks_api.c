#include <assert.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/hooks_api.h"
#include "mud/lua/ref.h"

#define HOOKS_LIB_NAME "hooks"

static int lua_hooks_on_startup(lua_State* l);
static int lua_hooks_on_shutdown(lua_State* l);
static int lua_hooks_on_entities_loaded(lua_State* l);
static int lua_hooks_on_commands_loaded(lua_State* l);
static int lua_hooks_on_command_groups_loaded(lua_State* l);
static int lua_hooks_on_actions_loaded(lua_State* l);
static int lua_hooks_on_player_connected(lua_State* l);
static int lua_hooks_on_player_disconnected(lua_State* l);
static int lua_hooks_on_player_input(lua_State* l);

static const struct luaL_Reg hooks_lib[] = {
  { "on_startup", lua_hooks_on_startup },
  { "on_shutdown", lua_hooks_on_shutdown },
  { "on_entities_loaded", lua_hooks_on_entities_loaded },
  { "on_commands_loaded", lua_hooks_on_commands_loaded },
  { "on_command_groups_loaded", lua_hooks_on_command_groups_loaded },
  { "on_actions_loaded", lua_hooks_on_actions_loaded },
  { "on_player_connected", lua_hooks_on_player_connected },
  { "on_player_disconnected", lua_hooks_on_player_disconnected },
  { "on_player_input", lua_hooks_on_player_input },
  { NULL, NULL }
};

/**
 * Registers the hooks API under lunac.api.hooks.
 **/
int lua_hooks_register_api(lua_State* l) {
  lua_push_api_table(l);

  lua_pushstring(l, HOOKS_LIB_NAME);
  luaL_newlib(l, hooks_lib);

  lua_rawset(l, -3);

  return 0;
}

/**
 * Stores a function to be called when the engine finishes initialising.
 *
 * lunac.api.hooks.on_startup(fn)
 **/
static int lua_hooks_on_startup(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_startup);
  game->hooks->on_startup = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called when the engine shuts down.
 *
 * lunac.api.hooks.on_shutdown(fn)
 **/
static int lua_hooks_on_shutdown(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_shutdown);
  game->hooks->on_shutdown = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called after entities are loaded from the database.
 *
 * lunac.api.hooks.on_entities_loaded(fn)
 **/
static int lua_hooks_on_entities_loaded(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_entities_loaded);
  game->hooks->on_entities_loaded = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called after commands are loaded from the database.
 *
 * lunac.api.hooks.on_commands_loaded(fn)
 **/
static int lua_hooks_on_commands_loaded(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_commands_loaded);
  game->hooks->on_commands_loaded = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called after command groups are loaded from the database.
 *
 * lunac.api.hooks.on_command_groups_loaded(fn)
 **/
static int lua_hooks_on_command_groups_loaded(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_command_groups_loaded);
  game->hooks->on_command_groups_loaded = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called after actions are loaded from the database.
 *
 * lunac.api.hooks.on_actions_loaded(fn)
 **/
static int lua_hooks_on_actions_loaded(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_actions_loaded);
  game->hooks->on_actions_loaded = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called when a player connects.
 *
 * lunac.api.hooks.on_player_connected(fn)
 **/
static int lua_hooks_on_player_connected(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_player_connected);
  game->hooks->on_player_connected = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called when a player disconnects.
 *
 * lunac.api.hooks.on_player_disconnected(fn)
 **/
static int lua_hooks_on_player_disconnected(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_player_disconnected);
  game->hooks->on_player_disconnected = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}

/**
 * Stores a function to be called when a player sends input.
 *
 * lunac.api.hooks.on_player_input(fn)
 **/
static int lua_hooks_on_player_input(lua_State* l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);

  game_t* game = lua_get_game(l);

  lua_free_lua_ref_t(game->hooks->on_player_input);
  game->hooks->on_player_input = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 0;
}
