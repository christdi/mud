#include <assert.h>
#include <string.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/action.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/system.h"
#include "mud/log.h"
#include "mud/json.h"
#include "mud/lua/common.h"
#include "mud/lua/event.h"
#include "mud/lua/hooks.h"
#include "mud/lua/struct.h"
#include "mud/narrator.h"
#include "mud/player.h"
#include "mud/state.h"
#include "mud/task.h"
#include "mud/util/muduuid.h"

#define ON_STARTUP_HOOK_FUNCTION "main"
#define ON_SHUTDOWN_HOOK_FUNCTION "shutdown"

#define ON_ENTITIES_LOADED_HOOK_FUNCTION "entities_loaded"
#define ON_COMMANDS_LOADED_HOOK_FUNCTION "commands_loaded"
#define ON_ACTIONS_LOADED_HOOK_FUNCTION "actions_loaded"

#define ON_PLAYER_CONNECTED_HOOK_FUNCTION "player_connected"
#define ON_PLAYER_DISCONNECTED_HOOK_FUNCTION "player_disconnected"
#define ON_PLAYER_INPUT_HOOK_FUNCTION "player_input"

#define NARRATE_EVENT_HOOK_FUNCTION "narrate"

#define STATE_ENTER_HOOK_FUNCTION "on_enter"
#define STATE_EXIT_HOOK_FUNCTION "on_exit"
#define STATE_INPUT_HOOK_FUNCTION "on_input"
#define STATE_OUTPUT_HOOK_FUNCTION "on_output"
#define STATE_EVENT_HOOK_FUNCTION "on_event"
#define STATE_GMCP_HOOK_FUNCTION "on_gmcp"

#define SYSTEM_EXECUTE_HOOK_FUNCTION "execute"

/**
 * Hook method called when the Lua state has been successfully initialised.
 *
 * l - Lua state instance
 *
 * Returns 0 on success or -1 on error
 **/
int lua_call_startup_hook(lua_State* l) {
  assert(l);

  if (lua_getglobal(l, ON_STARTUP_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  if (lua_pcall(l, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling on startup hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Hook method called when the engine is shuting down.
**/
int lua_call_shutdown_hook(lua_State *l) {
  assert(l);

  if (lua_getglobal(l, ON_SHUTDOWN_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  if (lua_pcall(l, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling on shutdown hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Hook method called when the engine has loaded all entities from persistence.
 *
 * l - Lua state instance
 * entities - A linked list of entity_t structs
 *
 * Returns 0 on success or -1 on failure
 **/
int lua_call_entities_loaded_hook(lua_State* l, linked_list_t* entities) {
  assert(l);
  assert(entities);

  if (lua_getglobal(l, ON_ENTITIES_LOADED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_newtable(l);

  entity_t* entity = NULL;
  int index = 1;

  it_t it = list_begin(entities);

  while ((entity = (entity_t*)it_get(it)) != NULL) {
    lua_pushnumber(l, index);
    lua_push_entity(l, entity);

    lua_rawset(l, -3);

    it = it_next(it);
    index++;
  }

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling entities loaded hook [%s]", lua_tostring(l, -1));
    return -1;
  }

  return 0;
}

/**
 * Hook method called when the engine has loaded all commands from persistence.
 *
 * l - Lua state instance
 * entities - A linked list of command_t structs
 *
 * Returns 0 on success or -1 on failure
 **/
int lua_call_commands_loaded_hook(lua_State* l, linked_list_t* commands) {
  assert(l);
  assert(commands);

  if (lua_getglobal(l, ON_COMMANDS_LOADED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_newtable(l);

  command_t* command = NULL;
  int index = 1;

  it_t it = list_begin(commands);

  while ((command = it_get(it)) != NULL) {
    lua_pushnumber(l, index);
    lua_push_command(l, command);

    lua_rawset(l, -3);

    it = it_next(it);
    index++;
  }

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling actions loaded hook [%s]", lua_tostring(l, -1));
    return -1;
  }

  return 0;
}

/**
 * Hook method called when the engine has loaded all actions from persistence.
 *
 * l - Lua state instance
 * entities - A linked list of action_t structs
 *
 * Returns 0 on success or -1 on failure
 **/
int lua_call_actions_loaded_hook(lua_State* l, linked_list_t* actions) {
  assert(l);
  assert(actions);

  if (lua_getglobal(l, ON_ACTIONS_LOADED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_newtable(l);

  action_t* action = NULL;
  int index = 1;

  it_t it = list_begin(actions);

  while ((action = it_get(it)) != NULL) {
    lua_pushnumber(l, index);
    lua_push_action(l, action);

    lua_rawset(l, -3);

    it = it_next(it);
    index++;
  }

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling actions loaded hook [%s]", lua_tostring(l, -1));
    return -1;
  }

  return 0;
}

/**
 * Hook method called when the engine has accepted a connection and created a new player.
 *
 * l - Lua state instance
 * player - New player instance
 *
 * Returns 0 on success or -1 on failure.
 **/
int lua_call_player_connected_hook(lua_State* l, player_t* player) {
  assert(l);
  assert(player);

  if (lua_getglobal(l, ON_PLAYER_CONNECTED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_push_player(l, player);

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player connected hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Hook method called when the engine has detected that a player has disconnected.
 *
 * l - Lua state instance
 * player - New player instance
 *
 * Returns 0 on success or -1 on failure.
 **/
int lua_call_player_disconnected_hook(lua_State* l, player_t* player) {
  assert(l);
  assert(player);

  if (lua_getglobal(l, ON_PLAYER_DISCONNECTED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_push_player(l, player);

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player disconnected hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * TODO(Chris I)
 **/
int lua_call_player_input_hook(lua_State* l, player_t* player, const char* input) {
  assert(l);
  assert(player);
  assert(input);

  if (lua_getglobal(l, ON_PLAYER_INPUT_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_pushlightuserdata(l, player);
  lua_pushstring(l, input);

  if (lua_pcall(l, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player input hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the narrate method associated with the provided Lua narrator module.
 *
 * Parameters
 *   l - The Lua state
 *   player - The player who is being narrated to
 *   narrator - Narrator struct containing ref to Lua module
 *   event - The event to be narrated
 **/
int lua_call_narrate_event_hook(lua_State* l, player_t* player, narrator_t* narrator, lua_event_data_t* event) {
  assert(l);
  assert(player);
  assert(narrator);
  assert(event);

  lua_rawgeti(l, LUA_REGISTRYINDEX, narrator->ref); // 1 - narrator module table
  lua_pushstring(l, NARRATE_EVENT_HOOK_FUNCTION); // 1 - narrator module, table, 2 = "narrate"

  if (lua_gettable(l, -2) != LUA_TFUNCTION) { // 1 - narrator module, 2 = narrate function
    lua_pop(l, 2);

    return 0;
  }

  lua_remove(l, -2); // 1 = narrate function
  lua_push_player(l, player); // 1 = narrate function, 2 = player table
  lua_rawgeti(l, LUA_REGISTRYINDEX, event->ref); // 1 = narrate function, 2 = player table, 3 = event data table

  if (lua_pcall(l, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling narrate hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_enter method associated with the provided Lua state module.
 *
 * Parameters
 *   l - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_enter_hook(lua_State* l, player_t* player, state_t* state) {
  assert(l);
  assert(player);
  assert(state);

  lua_rawgeti(l, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(l, STATE_ENTER_HOOK_FUNCTION); // -2 = state module table, -1 = on_enter method name

  if (lua_gettable(l, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_enter method
    lua_pop(l, 2);

    return 0;
  }

  lua_remove(l, -2); // 1 = on_enter method
  lua_push_player(l, player); // 1 = on_enter method, 2 = player table

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state enter hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_exit method associated with the provided Lua state module.
 *
 * Parameters
 *   l - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_exit_hook(lua_State* l, player_t* player, state_t* state) {
  assert(l);
  assert(player);
  assert(state);

  lua_rawgeti(l, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(l, STATE_EXIT_HOOK_FUNCTION); // -2 = state module table, -1 = on_exit method name

  if (lua_gettable(l, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_exit method
    lua_pop(l, 2);

    return 0;
  }

  lua_remove(l, -2); // -1 = on_exit method
  lua_push_player(l, player); // -2 = on_exit method, -1 = player table

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state exit hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_input method associated with the provided Lua state module.
 *
 * Parameters
 *   l - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *   input - The input being passed to the state
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_input_hook(lua_State* l, player_t* player, state_t* state, const char* input) {
  assert(l);
  assert(player);
  assert(state);

  lua_rawgeti(l, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(l, STATE_INPUT_HOOK_FUNCTION); // -2 = state module table, -1 = on_input method name

  if (lua_gettable(l, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_input method
    lua_pop(l, 2);

    return 0;
  }

  lua_remove(l, -2); // -1 = on_input method
  lua_push_player(l, player); // -2 = on_input method, -1 = player table
  lua_pushstring(l, input); // -3 = on_input method, -2 = player ptable, -1 = input

  if (lua_pcall(l, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state input hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_output method associated with the provided Lua state module.
 *
 * Parameters
 *   l - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_output_hook(lua_State* l, player_t* player, state_t* state, const char* output) {
  assert(l);
  assert(player);
  assert(state);

  lua_rawgeti(l, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(l, STATE_OUTPUT_HOOK_FUNCTION); // -2 = state module table, -1 = on_input method name

  if (lua_gettable(l, -2) != LUA_TFUNCTION) { // 1 = state module table, 2 = on_input method
    lua_pop(l, 2);

    return 0;
  }

  lua_remove(l, -2); // -1 = on_input method
  lua_push_player(l, player); // -2 = on_input method, -1 = player table
  lua_pushstring(l, output); // -3 = on_input method, -2 = player table, -1 = output string

  if (lua_pcall(l, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state output hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_event method associated with the provided Lua state module.
 *
 * Parameters
 *   l - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *   event - The event that has occurred
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_event_hook(lua_State* l, player_t* player, state_t* state, event_t* event) {
  assert(l);
  assert(player);
  assert(state);
  assert(event);

  lua_rawgeti(l, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(l, STATE_EVENT_HOOK_FUNCTION); // -2 = state module table, -1 = on_event method name

  if (lua_gettable(l, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_event method
    lua_pop(l, 2);

    return 0;
  }

  lua_remove(l, -2); // -1 = on_event method

  lua_push_player(l, player); // -2 = on_event method, -1 = player table
  lua_pushlightuserdata(l, event); // -3 = on_event method, -2 = player table, -1 = event pointer

  if (lua_pcall(l, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state event hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the on_event method associated with the provided Lua state module.
 *
 * Parameters
 *   l - The Lua state
 *   player - The player whom we're calling this state for
 *   state - The state we're calling
 *   topic - the topic of the GMCP message
 *   msg - the message of the GMCP message, may be null
 *
 * Returns 0 on success or returns luaL_error on error.
 **/
int lua_call_state_gmcp_hook(lua_State* l, player_t* player, state_t* state, const char* topic, const char* msg) {
  assert(l);
  assert(player);
  assert(state);
  assert(topic);

  lua_rawgeti(l, LUA_REGISTRYINDEX, state->ref); // -1 = state module table
  lua_pushstring(l, STATE_GMCP_HOOK_FUNCTION); // -2 = state module table, -1 = on_gmcp method name

  if (lua_gettable(l, -2) != LUA_TFUNCTION) { // -2 = state module table, -1 = on_gmcp method
    lua_pop(l, 2);

    return 0;
  }

  lua_remove(l, -2); // -1 = on_gmcp method

  lua_push_player(l, player); // -2 = on_gmcp method, -1 = player table
  lua_pushstring(l, topic); // -3 = on_gmcp method, -2 = player table, -1 = topic

  if (msg != NULL) {
    json_node_t* node = NULL;

    if ((node = json_deserialize(msg, strlen(msg))) == NULL) {
      LOG(ERROR, "Failed to deserialize GMCP JSON data [%s]", msg);
      lua_pop(l, 3);
      
      return -1;
    }

    lua_push_json_node(l, node); // -4 = on_gmcp method, -3 player table, -2 = topic, -1 msg
    json_free_json_node_t(node);
  }

  if (lua_pcall(l, msg == NULL ? 2 : 3, 0, 0) != 0) {
    LOG(ERROR, "Error when calling state gmcp hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  

  return 0;
}

/**
 * Calls the execute method of a Lua table associated with a system_t instance.
 *
 * l - Lua state
 * system - System we're calling
 *
 * Returns 0 on success or -1 on failure
**/
int lua_call_system_execute_hook(lua_State* l, system_t* system) {
  assert(l);
  assert(system);

  lua_rawgeti(l, LUA_REGISTRYINDEX, system->ref);
  lua_pushstring(l, SYSTEM_EXECUTE_HOOK_FUNCTION);

  if (lua_gettable(l, -2) != LUA_TFUNCTION) {
    lua_pop(l, 2);

    LOG(ERROR, "Lua system module did not define an execute function");

    return -1;
  }

  lua_remove(l, -2);

  if (lua_pcall(l, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling system execute hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * Calls the Lua function associated with a reference within a task.
 *
 * l - Lua state
 * task - the task to be executed
**/
int lua_call_task_execute_hook(lua_State* l, task_t* task) {
  assert(l);
  assert(task);

  lua_rawgeti(l, LUA_REGISTRYINDEX, task->ref);

  luaL_checktype(l, -1, LUA_TFUNCTION);

  if (lua_pcall(l, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling task [%s]", task->name);

    return -1;
  }

  luaL_unref(l, LUA_REGISTRYINDEX, task->ref);

  return 0;
}
