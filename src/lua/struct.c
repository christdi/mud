#include <assert.h>
#include <string.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/action.h"
#include "mud/command.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/entity.h"
#include "mud/ecs/system.h"
#include "mud/json.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/struct.h"
#include "mud/player.h"
#include "mud/task.h"
#include "mud/util/muduuid.h"

#define PTR_FIELD "_ptr"
#define TYPE_FIELD "_type"
#define UUID_FIELD "uuid"

#define PLAYER_USER_UUID_FIELD "user_uuid"
#define PLAYER_USERNAME_FIELD "username"

#define COMMAND_NAME_FIELD "name"
#define COMMAND_SCRIPT_UUID_FIELD "script"

#define COMMAND_GROUP_DESCRIPTION_FIELD "description"
#define COMMAND_GROUP_COMMANDS_FIELD "commands"

#define ACTION_NAME_FIELD "name"
#define ACTION_SCRIPT_UUID_FIELD "script"

#define SYSTEM_NAME_FIELD "name"
#define SYSTEM_ENABLED_FIELD "enabled"

#define TASK_NAME_FIELD "name"
#define TASK_EXECUTE_AT "execute_at"

#define JSON_NODE_VALUE_FIELD "node"

static void lua_push_json_value(lua_State* lua, json_node_t* node);

/**
 * Converts an entity structure to a Lua table and pushes iter on top of the stack.
 *
 * lua - Lua state instance
 * entity - entity to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_entity(lua_State* lua, entity_t* entity) {
  assert(lua);
  assert(entity);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_ENTITY);
  lua_rawset(lua, -3);

  lua_pushstring(lua, PTR_FIELD);
  lua_pushlightuserdata(lua, entity);
  lua_rawset(lua, -3);

  lua_pushstring(lua, UUID_FIELD);
  lua_pushstring(lua, uuid_str(&entity->id));
  lua_rawset(lua, -3);
}

/**
 * Converts a player structure to a Lua table and pushes iter on top of the stack.
 *
 * lua - Lua state instance
 * player - player to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_player(lua_State* lua, player_t* player) {
  assert(lua);
  assert(player);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_PLAYER);
  lua_rawset(lua, -3);

  lua_pushstring(lua, PTR_FIELD);
  lua_pushlightuserdata(lua, player);
  lua_rawset(lua, -3);

  lua_pushstring(lua, UUID_FIELD);
  lua_pushstring(lua, uuid_str(&player->uuid));
  lua_rawset(lua, -3);

  lua_pushstring(lua, PLAYER_USER_UUID_FIELD);
  lua_pushstring(lua, uuid_str(&player->user_uuid));
  lua_rawset(lua, -3);

  lua_pushstring(lua, PLAYER_USERNAME_FIELD);
  lua_pushstring(lua, player->username);
  lua_rawset(lua, -3);
}

/**
 * Converts a command structure to a Lua table and pushes iter on top of the stack.
 *
 * lua - Lua state instance
 * command - command to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_command(lua_State* lua, command_t* command) {
  assert(lua);
  assert(command);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_COMMAND);
  lua_rawset(lua, -3);

  lua_pushstring(lua, PTR_FIELD);
  lua_pushlightuserdata(lua, command);
  lua_rawset(lua, -3);

  lua_pushstring(lua, UUID_FIELD);
  lua_pushstring(lua, uuid_str(&command->uuid));
  lua_rawset(lua, -3);

  lua_pushstring(lua, COMMAND_NAME_FIELD);
  lua_pushstring(lua, command->name);
  lua_rawset(lua, -3);

  lua_pushstring(lua, COMMAND_SCRIPT_UUID_FIELD);
  lua_pushstring(lua, uuid_str(&command->script));
  lua_rawset(lua, -3);
}

/**
 * Converts a command group structure to a Lua table and pushes iter on top of the stack.
 * 
 * lua - Lua state instance
 * group - command group to be converted
**/
void lua_push_command_group(lua_State* lua, command_group_t* group) {
  assert(lua);
  assert(group);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_COMMAND_GROUP);
  lua_rawset(lua, -3);

  lua_pushstring(lua, PTR_FIELD);
  lua_pushlightuserdata(lua, group);
  lua_rawset(lua, -3);

  lua_pushstring(lua, UUID_FIELD);
  lua_pushstring(lua, uuid_str(&group->uuid));
  lua_rawset(lua, -3);

  lua_pushstring(lua, COMMAND_GROUP_DESCRIPTION_FIELD);
  lua_pushstring(lua, group->description);
  lua_rawset(lua, -3);

  int index = 1;

  char* uuid = NULL;
  it_t iter = list_begin(group->commands);

  lua_pushstring(lua, COMMAND_GROUP_COMMANDS_FIELD);
  lua_newtable(lua);

  while ((uuid = it_get(iter) ) != NULL) {
    lua_pushnumber(lua, index);
    lua_pushstring(lua, uuid);
    lua_rawset(lua, -3);

    iter = it_next(iter);

    index++;
  }
  
  lua_rawset(lua, -3);
}

/**
 * Converts an action structure to a Lua table and pushes iter on top of the stack.
 *
 * lua - Lua state instance
 * action - action to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_action(lua_State* lua, action_t* action) {
  assert(lua);
  assert(action);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_ACTION);
  lua_rawset(lua, -3);

  lua_pushstring(lua, PTR_FIELD);
  lua_pushlightuserdata(lua, action);
  lua_rawset(lua, -3);

  lua_pushstring(lua, UUID_FIELD);
  lua_pushstring(lua, uuid_str(&action->uuid));
  lua_rawset(lua, -3);

  lua_pushstring(lua, ACTION_NAME_FIELD);
  lua_pushstring(lua, action->name);
  lua_rawset(lua, -3);

  lua_pushstring(lua, ACTION_SCRIPT_UUID_FIELD);
  lua_pushstring(lua, uuid_str(&action->script));
  lua_rawset(lua, -3);
}

/**
 * Converts a system structure to a Lua table and pushes iter on top of the stack.
 *
 * lua - Lua state instance
 * system - system to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_system(lua_State* lua, system_t* system) {
  assert(lua);
  assert(system);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_SYSTEM);
  lua_rawset(lua, -3);

  lua_pushstring(lua, PTR_FIELD);
  lua_pushlightuserdata(lua, system);
  lua_rawset(lua, -3);

  lua_pushstring(lua, UUID_FIELD);
  lua_pushstring(lua, uuid_str(&system->uuid));
  lua_rawset(lua, -3);

  lua_pushstring(lua, SYSTEM_NAME_FIELD);
  lua_pushstring(lua, system->name);
  lua_rawset(lua, -3);

  lua_pushstring(lua, SYSTEM_ENABLED_FIELD);
  lua_pushboolean(lua, system->enabled);
  lua_rawset(lua, -3);
}

/**
 * Converts a task structure to a Lua table and pushes iter on top of the stack.
 *
 * lua - Lua state instance
 * task - task to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_task(lua_State* lua, task_t* task) {
  assert(lua);
  assert(task);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_TASK);
  lua_rawset(lua, -3);

  lua_pushstring(lua, PTR_FIELD);
  lua_pushlightuserdata(lua, task);
  lua_rawset(lua, -3);

  lua_pushstring(lua, UUID_FIELD);
  lua_pushstring(lua, uuid_str(&task->uuid));
  lua_rawset(lua, -3);

  lua_pushstring(lua, TASK_NAME_FIELD);
  lua_pushstring(lua, task->name);
  lua_rawset(lua, -3);

  lua_pushstring(lua, TASK_EXECUTE_AT);
  lua_pushnumber(lua, task->execute_in);
  lua_rawset(lua, -3);
}

/**
 * Converts a json_node_t instance to a Lua table and pushes iter on top of the stack.
 *
 * lua - Lua state instance
 * task - task to be converted
 *
 * Returns 0 on success or -1 on failure
 **/
void lua_push_json_node(lua_State* lua, json_node_t* node) {
  assert(lua);
  assert(node);

  lua_newtable(lua);

  lua_pushstring(lua, TYPE_FIELD);
  lua_pushnumber(lua, STRUCT_JSON_NODE);
  lua_rawset(lua, -3);

  lua_pushstring(lua, JSON_NODE_VALUE_FIELD);
  lua_push_json_value(lua, node);
  lua_rawset(lua, -3);
}

/**
 * Module internal recursive method to step through a json_node_t and assign
 * them to a table on the stack.
**/
void lua_push_json_value(lua_State* lua, json_node_t* node) {
  assert(lua);
  assert(node);
  
  int index = 1;

  switch(node->type) {
    case OBJECT:
      lua_newtable(lua);

      for (json_node_t* child = node->value->children; child != NULL; child = child->next) {
        lua_pushstring(lua, child->key);
        lua_push_json_value(lua, child);
        lua_rawset(lua, -3);
      }

      break;

    case ARRAY:
      lua_newtable(lua);

      for (json_node_t* item = node->value->array; item != NULL; item = item->next) {
        lua_pushnumber(lua, index);
        lua_push_json_value(lua, item);
        lua_rawset(lua, -3);
        index++;
      }

      break;

    case STRING:
      lua_pushstring(lua, node->value->str);

      break;

    case NUMBER:
      lua_pushnumber(lua, node->value->number);

      break;

    case BOOLEAN:
      lua_pushboolean(lua, node->value->boolean);

      break;

    case NIL:
      lua_pushlightuserdata(lua, NULL);

      break;

    default:
      break;
  }
}

/**
 * Extracts the pointer to an entity_t from the table on top of the stack.
 *
 * lua - Lua state instance
 *
 * Returns the entity_t pointer or null
 **/
entity_t* lua_to_entity(lua_State* lua, int index) {
  assert(lua);

  luaL_checktype(lua, index, LUA_TTABLE);
  lua_pushstring(lua, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(lua, table_index);

  struct_type_t type = luaL_checknumber(lua, -1);
  lua_pop(lua, 1);

  if (type != STRUCT_ENTITY) {
    LOG(ERROR, "Could not convert lua table to entity as type was not entity");

    return NULL;
  }

  lua_pushstring(lua, PTR_FIELD);
  lua_rawget(lua, table_index);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  entity_t* entity = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  return entity;
}

/**
 * Extracts the pointer to an player from the table on top of the stack.
 *
 * lua - Lua state instance
 *
 * Returns the player_t pointer or null
 **/
player_t* lua_to_player(lua_State* lua, int index) {
  assert(lua);

  luaL_checktype(lua, index, LUA_TTABLE);
  lua_pushstring(lua, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(lua, table_index);

  struct_type_t type = luaL_checknumber(lua, -1);
  lua_pop(lua, 1);

  if (type != STRUCT_PLAYER) {
    LOG(ERROR, "Could not convert lua table to player as type was not player");

    return NULL;
  }

  lua_pushstring(lua, PTR_FIELD);
  lua_rawget(lua, table_index);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  player_t* player = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  return player;
}

/**
 * Extracts the pointer to a command from the table on top of the stack.
 *
 * lua - Lua state instance
 *
 * Returns the command_t pointer or null
 **/
command_t* lua_to_command(lua_State* lua, int index) {
  assert(lua);

  luaL_checktype(lua, index, LUA_TTABLE);
  lua_pushstring(lua, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(lua, table_index);

  struct_type_t type = luaL_checknumber(lua, -1);
  lua_pop(lua, 1);

  if (type != STRUCT_COMMAND) {
    LOG(ERROR, "Could not convert lua table to command as type was not command");

    return NULL;
  }

  lua_pushstring(lua, PTR_FIELD);
  lua_rawget(lua, table_index);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  command_t* command = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  return command;
}

/**
 * Extracts the pointer to a command group from the table on top of the stack.
 *
 * lua - Lua state instance
 *
 * Returns the command_group_t pointer or null
**/
command_group_t* lua_to_command_group(lua_State* lua, int index) {
  assert(lua);

  luaL_checktype(lua, index, LUA_TTABLE);
  lua_pushstring(lua, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(lua, table_index);

  struct_type_t type = luaL_checknumber(lua, -1);
  lua_pop(lua, 1);

  if (type != STRUCT_COMMAND_GROUP) {
    LOG(ERROR, "Could not convert lua table to command group as type was not command group");

    return NULL;
  }

  lua_pushstring(lua, PTR_FIELD);
  lua_rawget(lua, table_index);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  command_group_t* command_group = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  return command_group;  
}

/**
 * Extracts the pointer to an action from the table on top of the stack.
 *
 * lua - Lua state instance
 *
 * Returns the action_t pointer or null
 **/
action_t* lua_to_action(lua_State* lua, int index) {
  assert(lua);

  luaL_checktype(lua, index, LUA_TTABLE);
  lua_pushstring(lua, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(lua, table_index);

  struct_type_t type = luaL_checknumber(lua, -1);
  lua_pop(lua, 1);

  if (type != STRUCT_ACTION) {
    LOG(ERROR, "Could not convert lua table to action as type was not action");

    return NULL;
  }

  lua_pushstring(lua, PTR_FIELD);
  lua_rawget(lua, table_index);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  action_t* action = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  return action;
}

/**
 * Extracts the pointer to a system from the table on top of the stack.
 *
 * lua - Lua state instance
 *
 * Returns the system_t pointer or null
 **/
system_t* lua_to_system(lua_State* lua, int index) {
  assert(lua);

  luaL_checktype(lua, index, LUA_TTABLE);
  lua_pushstring(lua, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(lua, table_index);

  struct_type_t type = luaL_checknumber(lua, -1);
  lua_pop(lua, 1);

  if (type != STRUCT_SYSTEM) {
    LOG(ERROR, "Could not convert lua table to action as type was not action");

    return NULL;
  }

  lua_pushstring(lua, PTR_FIELD);
  lua_rawget(lua, table_index);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  system_t* system = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  return system;
}

/**
 * Extracts the pointer to a task from the table on top of the stack.
 *
 * lua - Lua state instance
 *
 * Returns the task_t pointer or null
 **/
task_t* lua_to_task(lua_State* lua, int index) {
  assert(lua);

  luaL_checktype(lua, index, LUA_TTABLE);
  lua_pushstring(lua, TYPE_FIELD);

  int table_index = index > 0 ? index : index - 1;
  lua_rawget(lua, table_index);

  struct_type_t type = luaL_checknumber(lua, -1);
  lua_pop(lua, 1);

  if (type != STRUCT_TASK) {
    LOG(ERROR, "Could not convert lua table to action as type was not struct");

    return NULL;
  }

  lua_pushstring(lua, PTR_FIELD);
  lua_rawget(lua, table_index);

  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  task_t* task = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  return task;
}

/**
 * Covnerts the table at index into a json_node_t representation.
 *
 * lua - Lua state instance
 *
 * Returns a pointer to an allocated json_node_t instance.
**/
json_node_t* lua_to_json_node(lua_State* lua, int index) {
  assert(lua);

  int type = lua_type(lua, index);
  json_node_t* node = NULL;

  if (type == LUA_TSTRING) {
    node = json_new_string(lua_tostring(lua, index));
  } else if (type == LUA_TNUMBER) {
    node = json_new_number(lua_tonumber(lua, index));
  } else if (type == LUA_TBOOLEAN) {
    node = json_new_boolean(lua_toboolean(lua, index));
  } else if (type == LUA_TNIL) {
    node = json_new_null();
  } else if (type == LUA_TLIGHTUSERDATA) {
    void* ctype = lua_touserdata(lua, index);

    if (ctype == NULL) {
      node = json_new_null();
    }
  } else if (type == LUA_TTABLE) {
    lua_pushnil(lua);

    int table_index = index > 0 ? index : index - 1;

    if (lua_next(lua, table_index) == 0) {
      return NULL;
    }

    int table_key_type = lua_type(lua, -2);

    if (table_key_type != LUA_TNUMBER && table_key_type != LUA_TSTRING)  {
      LOG(ERROR, "Unexpected key type [%s] in table, expected number or string", lua_typename(lua, table_key_type));

      lua_pop(lua, 2);

      return NULL;
    }

    if (table_key_type == LUA_TNUMBER) {
      node = json_new_json_node_t(ARRAY);
    } else if (table_key_type == LUA_TSTRING) {
      node = json_new_json_node_t(OBJECT);
    }

    lua_pop(lua, 2);
    lua_pushnil(lua);

    while (lua_next(lua, table_index) != 0) {
      if (lua_type(lua, -2) != table_key_type) {
        LOG(ERROR, "Expected key type [%s] but was [%s]", lua_typename(lua, table_key_type), lua_typename(lua, -2));

        lua_pop(lua, 2);

        return NULL;
      }

      if (table_key_type == LUA_TNUMBER) {
        json_attach_array(node, lua_to_json_node(lua, -1));
      } else {
        json_node_t* child = lua_to_json_node(lua, -1);
        child->key = strdup(lua_tostring(lua, -2));

        json_attach_child(node, child);
      }

      lua_pop(lua, 1);
    }
  }

  return node;
}
