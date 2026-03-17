#include <stdlib.h>

#include "mud/lua/hooks_api.h"
#include "mud/lua/ref.h"

/**
 * Allocates and returns a new lua_hooks_t with all refs initialised to NULL.
 **/
lua_hooks_t* lua_new_hooks_t(void) {
  return calloc(1, sizeof(lua_hooks_t));
}

/**
 * Frees an allocated lua_hooks_t and releases any stored Lua refs.
 **/
void lua_free_hooks_t(lua_hooks_t* hooks) {
  if (!hooks) {
    return;
  }

  lua_free_lua_ref_t(hooks->on_startup);
  lua_free_lua_ref_t(hooks->on_shutdown);
  lua_free_lua_ref_t(hooks->on_entities_loaded);
  lua_free_lua_ref_t(hooks->on_commands_loaded);
  lua_free_lua_ref_t(hooks->on_command_groups_loaded);
  lua_free_lua_ref_t(hooks->on_actions_loaded);
  lua_free_lua_ref_t(hooks->on_player_connected);
  lua_free_lua_ref_t(hooks->on_player_disconnected);
  lua_free_lua_ref_t(hooks->on_player_input);

  free(hooks);
}
