#include "lauxlib.h"
#include "lua.h"

#include "mud/action.h"
#include "mud/command.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/system.h"
#include "mud/json.h"
#include "mud/log.h"
#include "mud/lua/struct.h"
#include "mud/player.h"
#include "mud/task.h"
#include "mud/util/muduuid.h"

#include <assert.h>

#define PTR_FIELD "_ptr"
#define TYPE_FIELD "_type"
#define UUID_FIELD "uuid"

#define PLAYER_USER_UUID_FIELD "user_uuid"
#define PLAYER_USERNAME_FIELD "username"

#define COMMAND_NAME_FIELD "name"
#define COMMAND_SCRIPT_UUID_FIELD "script"

#define ACTION_NAME_FIELD "name"
#define ACTION_SCRIPT_UUID_FIELD "script"

#define SYSTEM_NAME_FIELD "name"
#define SYSTEM_ENABLED_FIELD "enabled"

#define TASK_NAME_FIELD "name"
#define TASK_EXECUTE_AT "execute_at"

#define JSON_NODE_VALUE_FIELD "node"

static void lua_push_json_value(lua_State* l, json_node_t* node);

/**
 * Converts an entity structure to a Lua table and pushes it on top of the stack.
 *
 * l - Lua state instance
 * entity - entity to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_entity(lua_State* l, entity_t* entity) {
  assert(l);
  assert(entity);

  lua_newtable(l);

  lua_pushstring(l, TYPE_FIELD);
  lua_pushnumber(l, STRUCT_ENTITY);
  lua_rawset(l, -3);

  lua_pushstring(l, PTR_FIELD);
  lua_pushlightuserdata(l, entity);
  lua_rawset(l, -3);

  lua_pushstring(l, UUID_FIELD);
  lua_pushstring(l, uuid_str(&entity->id));
  lua_rawset(l, -3);
}

/**
 * Converts a player structure to a Lua table and pushes it on top of the stack.
 *
 * l - Lua state instance
 * player - player to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_player(lua_State* l, player_t* player) {
  assert(l);
  assert(player);

  lua_newtable(l);

  lua_pushstring(l, TYPE_FIELD);
  lua_pushnumber(l, STRUCT_PLAYER);
  lua_rawset(l, -3);

  lua_pushstring(l, PTR_FIELD);
  lua_pushlightuserdata(l, player);
  lua_rawset(l, -3);

  lua_pushstring(l, UUID_FIELD);
  lua_pushstring(l, uuid_str(&player->uuid));
  lua_rawset(l, -3);

  lua_pushstring(l, PLAYER_USER_UUID_FIELD);
  lua_pushstring(l, uuid_str(&player->user_uuid));
  lua_rawset(l, -3);

  lua_pushstring(l, PLAYER_USERNAME_FIELD);
  lua_pushstring(l, player->username);
  lua_rawset(l, -3);
}

/**
 * Converts a command structure to a Lua table and pushes it on top of the stack.
 *
 * l - Lua state instance
 * command - command to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_command(lua_State* l, command_t* command) {
  assert(l);
  assert(command);

  lua_newtable(l);

  lua_pushstring(l, TYPE_FIELD);
  lua_pushnumber(l, STRUCT_COMMAND);
  lua_rawset(l, -3);

  lua_pushstring(l, PTR_FIELD);
  lua_pushlightuserdata(l, command);
  lua_rawset(l, -3);

  lua_pushstring(l, UUID_FIELD);
  lua_pushstring(l, uuid_str(&command->uuid));
  lua_rawset(l, -3);

  lua_pushstring(l, COMMAND_NAME_FIELD);
  lua_pushstring(l, command->name);
  lua_rawset(l, -3);

  lua_pushstring(l, COMMAND_SCRIPT_UUID_FIELD);
  lua_pushstring(l, uuid_str(&command->script));
  lua_rawset(l, -3);
}

/**
 * Converts an action structure to a Lua table and pushes it on top of the stack.
 *
 * l - Lua state instance
 * action - action to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_action(lua_State* l, action_t* action) {
  assert(l);
  assert(action);

  lua_newtable(l);

  lua_pushstring(l, TYPE_FIELD);
  lua_pushnumber(l, STRUCT_ACTION);
  lua_rawset(l, -3);

  lua_pushstring(l, PTR_FIELD);
  lua_pushlightuserdata(l, action);
  lua_rawset(l, -3);

  lua_pushstring(l, UUID_FIELD);
  lua_pushstring(l, uuid_str(&action->uuid));
  lua_rawset(l, -3);

  lua_pushstring(l, ACTION_NAME_FIELD);
  lua_pushstring(l, action->name);
  lua_rawset(l, -3);

  lua_pushstring(l, ACTION_SCRIPT_UUID_FIELD);
  lua_pushstring(l, uuid_str(&action->script));
  lua_rawset(l, -3);
}

/**
 * Converts a system structure to a Lua table and pushes it on top of the stack.
 *
 * l - Lua state instance
 * system - system to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_system(lua_State* l, system_t* system) {
  assert(l);
  assert(system);

  lua_newtable(l);

  lua_pushstring(l, TYPE_FIELD);
  lua_pushnumber(l, STRUCT_SYSTEM);
  lua_rawset(l, -3);

  lua_pushstring(l, PTR_FIELD);
  lua_pushlightuserdata(l, system);
  lua_rawset(l, -3);

  lua_pushstring(l, UUID_FIELD);
  lua_pushstring(l, uuid_str(&system->uuid));
  lua_rawset(l, -3);

  lua_pushstring(l, SYSTEM_NAME_FIELD);
  lua_pushstring(l, system->name);
  lua_rawset(l, -3);

  lua_pushstring(l, SYSTEM_ENABLED_FIELD);
  lua_pushboolean(l, system->enabled);
  lua_rawset(l, -3);
}

/**
 * Converts a task structure to a Lua table and pushes it on top of the stack.
 *
 * l - Lua state instance
 * task - task to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_task(lua_State* l, task_t* task) {
  assert(l);
  assert(task);

  lua_newtable(l);

  lua_pushstring(l, TYPE_FIELD);
  lua_pushnumber(l, STRUCT_TASK);
  lua_rawset(l, -3);

  lua_pushstring(l, PTR_FIELD);
  lua_pushlightuserdata(l, task);
  lua_rawset(l, -3);

  lua_pushstring(l, UUID_FIELD);
  lua_pushstring(l, uuid_str(&task->uuid));
  lua_rawset(l, -3);

  lua_pushstring(l, TASK_NAME_FIELD);
  lua_pushstring(l, task->name);
  lua_rawset(l, -3);

  lua_pushstring(l, TASK_EXECUTE_AT);
  lua_pushnumber(l, task->execute_at);
  lua_rawset(l, -3);
}

/**
 * Converts a json_node_t instance to a Lua table and pushes it on top of the stack.
 *
 * l - Lua state instance
 * task - task to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_json_node(lua_State* l, json_node_t* node) {
  assert(l);
  assert(node);

  lua_newtable(l);

  lua_pushstring(l, TYPE_FIELD);
  lua_pushnumber(l, STRUCT_JSON_NODE);
  lua_rawset(l, -3);

  lua_pushstring(l, JSON_NODE_VALUE_FIELD);
  lua_push_json_value(l, node);
  lua_rawset(l, -3);
}

/**
 * Module internal recursive method to step through a json_node_t and assign
 * them to a table on the stack.
**/
void lua_push_json_value(lua_State* l, json_node_t* node) {
  assert(l);
  assert(node);
  
  int index = 1;

  switch(node->type) {
    case OBJECT:
      lua_newtable(l);

      for (json_node_t* child = node->value->children; child != NULL; child = child->next) {
        lua_pushstring(l, child->key);
        lua_push_json_value(l, child);
        lua_rawset(l, -3);
      }

      break;

    case ARRAY:
      lua_newtable(l);

      for (json_node_t* item = node->value->array; item != NULL; item = item->next) {
        lua_pushnumber(l, index);
        lua_push_json_value(l, item);
        lua_rawset(l, -3);
        index++;
      }

      break;

    case STRING:
      lua_pushstring(l, node->value->str);

      break;

    case NUMBER:
      lua_pushnumber(l, node->value->number);

      break;

    case BOOLEAN:
      lua_pushboolean(l, node->value->boolean);

      break;

    case NIL:
      lua_pushnil(l);

      break;

    default:
      break;
  }
}

/**
 * Extracts the pointer to an entity_t from the table on top of the stack.
 *
 * l - Lua state instance
 *
 * Returns the entity_t pointer or null
 **/
entity_t* lua_to_entity(lua_State* l, int index) {
  assert(l);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_pushstring(l, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(l, table_index);

  struct_type_t type = luaL_checknumber(l, -1);
  lua_pop(l, 1);

  if (type != STRUCT_ENTITY) {
    LOG(ERROR, "Could not convert lua table to entity as type was not entity");

    return NULL;
  }

  lua_pushstring(l, PTR_FIELD);
  lua_rawget(l, table_index);

  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  entity_t* entity = lua_touserdata(l, -1);
  lua_pop(l, 1);

  return entity;
}

/**
 * Extracts the pointer to an player from the table on top of the stack.
 *
 * l - Lua state instance
 *
 * Returns the player_t pointer or null
 **/
player_t* lua_to_player(lua_State* l, int index) {
  assert(l);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_pushstring(l, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(l, table_index);

  struct_type_t type = luaL_checknumber(l, -1);
  lua_pop(l, 1);

  if (type != STRUCT_PLAYER) {
    LOG(ERROR, "Could not convert lua table to player as type was not player");

    return NULL;
  }

  lua_pushstring(l, PTR_FIELD);
  lua_rawget(l, table_index);

  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  player_t* player = lua_touserdata(l, -1);
  lua_pop(l, 1);

  return player;
}

/**
 * Extracts the pointer to a command from the table on top of the stack.
 *
 * l - Lua state instance
 *
 * Returns the command_t pointer or null
 **/
command_t* lua_to_command(lua_State* l, int index) {
  assert(l);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_pushstring(l, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(l, table_index);

  struct_type_t type = luaL_checknumber(l, -1);
  lua_pop(l, 1);

  if (type != STRUCT_COMMAND) {
    LOG(ERROR, "Could not convert lua table to command as type was not command");

    return NULL;
  }

  lua_pushstring(l, PTR_FIELD);
  lua_rawget(l, table_index);

  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  command_t* command = lua_touserdata(l, -1);
  lua_pop(l, 1);

  return command;
}

/**
 * Extracts the pointer to an action from the table on top of the stack.
 *
 * l - Lua state instance
 *
 * Returns the action_t pointer or null
 **/
action_t* lua_to_action(lua_State* l, int index) {
  assert(l);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_pushstring(l, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(l, table_index);

  struct_type_t type = luaL_checknumber(l, -1);
  lua_pop(l, 1);

  if (type != STRUCT_ACTION) {
    LOG(ERROR, "Could not convert lua table to action as type was not action");

    return NULL;
  }

  lua_pushstring(l, PTR_FIELD);
  lua_rawget(l, table_index);

  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  action_t* action = lua_touserdata(l, -1);
  lua_pop(l, 1);

  return action;
}

/**
 * Extracts the pointer to a system from the table on top of the stack.
 *
 * l - Lua state instance
 *
 * Returns the system_t pointer or null
 **/
system_t* lua_to_system(lua_State* l, int index) {
  assert(l);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_pushstring(l, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(l, table_index);

  struct_type_t type = luaL_checknumber(l, -1);
  lua_pop(l, 1);

  if (type != STRUCT_SYSTEM) {
    LOG(ERROR, "Could not convert lua table to action as type was not action");

    return NULL;
  }

  lua_pushstring(l, PTR_FIELD);
  lua_rawget(l, table_index);

  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  system_t* system = lua_touserdata(l, -1);
  lua_pop(l, 1);

  return system;
}

/**
 * Extracts the pointer to a task from the table on top of the stack.
 *
 * l - Lua state instance
 *
 * Returns the task_t pointer or null
 **/
task_t* lua_to_task(lua_State* l, int index) {
  assert(l);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_pushstring(l, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(l, table_index);

  struct_type_t type = luaL_checknumber(l, -1);
  lua_pop(l, 1);

  if (type != STRUCT_TASK) {
    LOG(ERROR, "Could not convert lua table to action as type was not struct");

    return NULL;
  }

  lua_pushstring(l, PTR_FIELD);
  lua_rawget(l, table_index);

  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  task_t* task = lua_touserdata(l, -1);
  lua_pop(l, 1);

  return task;
}
