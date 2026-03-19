#include <assert.h>
#include <string.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/action.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/system.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/json.h"
#include "mud/lua/common.h"
#include "mud/lua/hooks.h"
#include "mud/lua/hooks_api.h"
#include "mud/lua/ref.h"
#include "mud/lua/struct.h"
#include "mud/player.h"
#include "mud/task.h"
#include "mud/util/muduuid.h"

#define NARRATE_EVENT_HOOK_FUNCTION "narrate"

#define STATE_ENTER_HOOK_FUNCTION "on_enter"
#define STATE_EXIT_HOOK_FUNCTION "on_exit"
#define STATE_INPUT_HOOK_FUNCTION "on_input"
#define STATE_OUTPUT_HOOK_FUNCTION "on_output"
#define STATE_EVENT_HOOK_FUNCTION "on_event"
#define STATE_GMCP_HOOK_FUNCTION "on_gmcp"

#define SYSTEM_EXECUTE_HOOK_FUNCTION "execute"

/**
 * Calls the startup hook if one has been registered.
 *
 * lua - Lua state instance
 *
 * Returns 0 on success or -1 on error
 **/
int lua_call_startup_hook(lua_State* lua) {
  assert(lua);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_startup == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_startup->ref);

  if (lua_pcall(lua, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling on startup hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the shutdown hook if one has been registered.
 *
 * lua - Lua state instance
 *
 * Returns 0 on success or -1 on error
 **/
int lua_call_shutdown_hook(lua_State* lua) {
  assert(lua);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_shutdown == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_shutdown->ref);

  if (lua_pcall(lua, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling on shutdown hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the entities_loaded hook if one has been registered.
 *
 * lua - Lua state instance
 * entities - A linked list of entity_t structs
 *
 * Returns 0 on success or -1 on failure
 **/
int lua_call_entities_loaded_hook(lua_State* lua, linked_list_t* entities) {
  assert(lua);
  assert(entities);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_entities_loaded == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_entities_loaded->ref);

  lua_newtable(lua);

  entity_t* entity = NULL;
  int index = 1;

  it_t iter = list_begin(entities);

  while ((entity = (entity_t*)it_get(iter)) != NULL) {
    lua_pushnumber(lua, index);
    lua_push_entity(lua, entity);

    lua_rawset(lua, -3);

    iter = it_next(iter);
    index++;
  }

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling entities loaded hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the commands_loaded hook if one has been registered.
 *
 * lua - Lua state instance
 * commands - A linked list of command_t structs
 *
 * Returns 0 on success or -1 on failure
 **/
int lua_call_commands_loaded_hook(lua_State* lua, linked_list_t* commands) {
  assert(lua);
  assert(commands);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_commands_loaded == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_commands_loaded->ref);

  lua_newtable(lua);

  command_t* command = NULL;
  int index = 1;

  it_t iter = list_begin(commands);

  while ((command = it_get(iter)) != NULL) {
    lua_pushnumber(lua, index);
    lua_push_command(lua, command);

    lua_rawset(lua, -3);

    iter = it_next(iter);
    index++;
  }

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling commands loaded hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the command_groups_loaded hook if one has been registered.
 *
 * lua - Lua state instance
 * command_groups - A linked list of command_group_t structs
 *
 * Returns 0 on success or -1 on failure
**/
int lua_call_command_groups_loaded_hook(lua_State* lua, linked_list_t* command_groups) {
  assert(lua);
  assert(command_groups);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_command_groups_loaded == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_command_groups_loaded->ref);

  lua_newtable(lua);

  command_group_t* command_group = NULL;

  int index = 1;
  it_t iter = list_begin(command_groups);

  while ((command_group = it_get(iter)) != NULL) {
    lua_pushnumber(lua, index);
    lua_push_command_group(lua, command_group);

    lua_rawset(lua, -3);

    iter = it_next(iter);

    index++;
  }

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling command groups loaded hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the actions_loaded hook if one has been registered.
 *
 * lua - Lua state instance
 * actions - A linked list of action_t structs
 *
 * Returns 0 on success or -1 on failure
 **/
int lua_call_actions_loaded_hook(lua_State* lua, linked_list_t* actions) {
  assert(lua);
  assert(actions);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_actions_loaded == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_actions_loaded->ref);

  lua_newtable(lua);

  action_t* action = NULL;
  int index = 1;

  it_t iter = list_begin(actions);

  while ((action = it_get(iter)) != NULL) {
    lua_pushnumber(lua, index);
    lua_push_action(lua, action);

    lua_rawset(lua, -3);

    iter = it_next(iter);
    index++;
  }

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling actions loaded hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the player_connected hook if one has been registered.
 *
 * lua - Lua state instance
 * player - New player instance
 *
 * Returns 0 on success or -1 on failure.
 **/
int lua_call_player_connected_hook(lua_State* lua, player_t* player) {
  assert(lua);
  assert(player);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_player_connected == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_player_connected->ref);

  lua_push_player(lua, player);

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player connected hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the player_disconnected hook if one has been registered.
 *
 * lua - Lua state instance
 * player - Disconnecting player instance
 *
 * Returns 0 on success or -1 on failure.
 **/
int lua_call_player_disconnected_hook(lua_State* lua, player_t* player) {
  assert(lua);
  assert(player);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_player_disconnected == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_player_disconnected->ref);

  lua_push_player(lua, player);

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player disconnected hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the player_input hook if one has been registered.
 *
 * lua - Lua state instance
 * player - player_t instance of the player who sent the command
 * input - Input string sent by the player
 *
 * Returns 0 on success or -1 on failure.
 **/
int lua_call_player_input_hook(lua_State* lua, player_t* player, const char* input) {
  assert(lua);
  assert(player);
  assert(input);

  game_t* game = lua_get_game(lua);

  if (game->hooks->on_player_input == NULL) {
    return 0;
  }

  lua_rawgeti(lua, LUA_REGISTRYINDEX, game->hooks->on_player_input->ref);

  lua_pushlightuserdata(lua, player);
  lua_pushstring(lua, input);

  if (lua_pcall(lua, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player input hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the narrate method associated with the provided Lua narrator module.
 *
 * Parameters
 *   lua - The Lua state
 *   player - The player who is being narrated to
 *   narrator - lua_ref_t to narrator in Lua state
 *   event - The event to be narrated
 **/
int lua_call_narrate_event_hook(lua_State* lua, player_t* player, lua_ref_t* narrator, lua_ref_t* event) {
  assert(lua);
  assert(player);
  assert(narrator);
  assert(event);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, narrator->ref); // 1 - narrator module table
  lua_pushstring(lua, NARRATE_EVENT_HOOK_FUNCTION); // 1 - narrator module, table, 2 = "narrate"

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) { // 1 - narrator module, 2 = narrate function
    lua_pop(lua, 2);

    return 0;
  }

  lua_remove(lua, -2); // 1 = narrate function
  lua_push_player(lua, player); // 1 = narrate function, 2 = player table
  lua_rawgeti(lua, LUA_REGISTRYINDEX, event->ref); // 1 = narrate function, 2 = player table, 3 = event data table

  if (lua_pcall(lua, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling narrate hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_enter method associated with the provided Lua state module.
 *
 * Parameters
 *   lua - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_enter_hook(lua_State* lua, player_t* player, lua_ref_t* state) {
  assert(lua);
  assert(player);
  assert(state);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(lua, STATE_ENTER_HOOK_FUNCTION); // -2 = state module table, -1 = on_enter method name

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_enter method
    lua_pop(lua, 2);

    return 0;
  }

  lua_remove(lua, -2); // 1 = on_enter method
  lua_push_player(lua, player); // 1 = on_enter method, 2 = player table

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state enter hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_exit method associated with the provided Lua state module.
 *
 * Parameters
 *   lua - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_exit_hook(lua_State* lua, player_t* player, lua_ref_t* state) {
  assert(lua);
  assert(player);
  assert(state);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(lua, STATE_EXIT_HOOK_FUNCTION); // -2 = state module table, -1 = on_exit method name

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_exit method
    lua_pop(lua, 2);

    return 0;
  }

  lua_remove(lua, -2); // -1 = on_exit method
  lua_push_player(lua, player); // -2 = on_exit method, -1 = player table

  if (lua_pcall(lua, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state exit hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_input method associated with the provided Lua state module.
 *
 * Parameters
 *   lua - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *   input - The input being passed to the state
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_input_hook(lua_State* lua, player_t* player, lua_ref_t* state, const char* input) {
  assert(lua);
  assert(player);
  assert(state);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(lua, STATE_INPUT_HOOK_FUNCTION); // -2 = state module table, -1 = on_input method name

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_input method
    lua_pop(lua, 2);

    return 0;
  }

  lua_remove(lua, -2); // -1 = on_input method
  lua_push_player(lua, player); // -2 = on_input method, -1 = player table
  lua_pushstring(lua, input); // -3 = on_input method, -2 = player ptable, -1 = input

  if (lua_pcall(lua, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state input hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_output method associated with the provided Lua state module.
 *
 * Parameters
 *   lua - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_output_hook(lua_State* lua, player_t* player, lua_ref_t* state, const char* output) {
  assert(lua);
  assert(player);
  assert(state);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(lua, STATE_OUTPUT_HOOK_FUNCTION); // -2 = state module table, -1 = on_input method name

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) { // 1 = state module table, 2 = on_input method
    lua_pop(lua, 2);

    return 0;
  }

  lua_remove(lua, -2); // -1 = on_input method
  lua_push_player(lua, player); // -2 = on_input method, -1 = player table
  lua_pushstring(lua, output); // -3 = on_input method, -2 = player table, -1 = output string

  if (lua_pcall(lua, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state output hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_event method associated with the provided Lua state module.
 *
 * Parameters
 *   lua - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *   event - The event that has occurred
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_event_hook(lua_State* lua, player_t* player, lua_ref_t* state, event_t* event) {
  assert(lua);
  assert(player);
  assert(state);
  assert(event);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(lua, STATE_EVENT_HOOK_FUNCTION); // -2 = state module table, -1 = on_event method name

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_event method
    lua_pop(lua, 2);

    return 0;
  }

  lua_remove(lua, -2); // -1 = on_event method

  lua_push_player(lua, player); // -2 = on_event method, -1 = player table
  lua_pushlightuserdata(lua, event); // -3 = on_event method, -2 = player table, -1 = event pointer

  if (lua_pcall(lua, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state event hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_gmcp method associated with the provided Lua state module.
 *
 * Parameters
 *   lua - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *   topic - the topic of the GMCP message
 *   msg - the message of the GMCP message, may be null
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_gmcp_hook(lua_State* lua, player_t* player, lua_ref_t* state, const char* topic, const char* msg) {
  assert(lua);
  assert(player);
  assert(state);
  assert(topic);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(lua, STATE_GMCP_HOOK_FUNCTION); // -2 = state module table, -1 = on_gmcp method name

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_gmcp method
    lua_pop(lua, 2);

    return 0;
  }

  lua_remove(lua, -2); // -1 = on_gmcp method

  lua_push_player(lua, player); // -2 = on_gmcp method, -1 = player table
  lua_pushstring(lua, topic); // -3 = on_gmcp method, -2 = player table, -1 = topic

  if (msg != NULL) {
    json_node_t* node = NULL;

    if ((node = json_deserialize(msg, strlen(msg))) == NULL) {
      LOG(ERROR, "Failed to deserialize GMCP JSON data [%s]", msg);
      lua_pop(lua, 3);

      return -1;
    }

    lua_push_json_node(lua, node); // -4 = on_gmcp method, -3 player table, -2 = topic, -1 msg
    json_free_json_node_t(node);
  }

  if (lua_pcall(lua, msg == NULL ? 2 : 3, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state gmcp hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the execute method of a Lua table associated with a system_t instance.
 *
 * lua - Lua state
 * system - System we're calling
 *
 * Returns 0 on success or -1 on failure
**/
int lua_call_system_execute_hook(lua_State* lua, system_t* system) {
  assert(lua);
  assert(system);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, system->ref->ref);
  lua_pushstring(lua, SYSTEM_EXECUTE_HOOK_FUNCTION);

  if (lua_gettable(lua, -2) != LUA_TFUNCTION) {
    lua_pop(lua, 2);

    LOG(ERROR, "Lua system module did not define an execute function");

    return -1;
  }

  lua_remove(lua, -2);

  if (lua_pcall(lua, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling system execute hook [%s]", lua_tostring(lua, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the Lua function associated with a reference within a task.
 *
 * lua - Lua state
 * task - the task to be executed
**/
int lua_call_task_execute_hook(lua_State* lua, task_t* task) {
  assert(lua);
  assert(task);

  lua_rawgeti(lua, LUA_REGISTRYINDEX, task->ref->ref);

  luaL_checktype(lua, -1, LUA_TFUNCTION);

  if (lua_pcall(lua, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling task [%s]", task->name);

    return -1;
  }

  return 0;
}
