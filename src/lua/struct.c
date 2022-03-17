#include "lauxlib.h"
#include "lua.h"

#include "mud/ecs/action.h"
#include "mud/ecs/entity.h"
#include "mud/log.h"
#include "mud/lua/struct.h"
#include "mud/player.h"
#include "mud/util/muduuid.h"

#include <assert.h>

#define PTR_FIELD "_ptr"
#define TYPE_FIELD "_type"
#define UUID_FIELD "uuid"

#define PLAYER_USER_UUID_FIELD "user_uuid"
#define PLAYER_USERNAME_FIELD "username"

#define ACTION_NAME_FIELD "name"
#define ACTION_SCRIPT_UUID_FIELD "script"

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
 * Extracts the pointer to an action from the table on top of the stack.
 *
 * l - Lua state instance
 *
 * Returns the axtion_t pointer or null
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
