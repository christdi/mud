#ifndef MUD_LUA_HOOKS_API_H
#define MUD_LUA_HOOKS_API_H

typedef struct lua_State lua_State;
typedef struct lua_ref lua_ref_t;

/**
 * Stores Lua function references for each engine lifecycle hook.
 * Refs are NULL until registered via lunac.api.hooks.*
 **/
typedef struct lua_hooks {
  lua_ref_t* on_startup;
  lua_ref_t* on_shutdown;
  lua_ref_t* on_entities_loaded;
  lua_ref_t* on_commands_loaded;
  lua_ref_t* on_command_groups_loaded;
  lua_ref_t* on_actions_loaded;
  lua_ref_t* on_player_connected;
  lua_ref_t* on_player_disconnected;
  lua_ref_t* on_player_input;
} lua_hooks_t;

lua_hooks_t* lua_new_hooks_t(void);
void lua_free_hooks_t(lua_hooks_t* hooks);

int lua_hooks_register_api(lua_State* l);

#endif
