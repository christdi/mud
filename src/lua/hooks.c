#include <assert.h>

#include "lua.h"

#include "mud/data/linked_list.h"
#include "mud/ecs/entity.h"
#include "mud/lua/hooks.h"
#include "mud/log.h"
#include "mud/player.h"

#define ON_STARTUP_HOOK_FUNCTION "main"
#define ON_ENTITIES_LOADED_HOOK_FUNCTION "entities_loaded"
#define ON_PLAYER_CONNECTED_HOOK_FUNCTION "player_connected"
#define ON_PLAYER_DISCONNECTED_HOOK_FUNCTION "player_disconnected"
#define ON_PLAYER_INPUT_HOOK_FUNCTION "player_input"

/**
 * TODO(Chris I)
**/
int lua_hook_on_startup(lua_State *l) {
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
 * TODO(Chris I)
 *
 * 4/-1 value
 * 3/-2 key
 * 2/-3 table
 * 1/-4 on_entities_loaded
**/
int lua_hook_on_entities_loaded(lua_State *l, linked_list_t* entities) {
  assert(l);
  assert(entities);

  if (lua_getglobal(l, ON_ENTITIES_LOADED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_newtable(l);

  it_t it = list_begin(entities);
  entity_t* entity = NULL;
  int index = 1;

  while ((entity = (entity_t*)it_get(it)) != NULL) {
    lua_pushnumber(l, index);
    lua_pushstring(l, entity->id.uuid);

    lua_rawset(l, 2);

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
 * TODO(Chris I)
**/
int lua_hook_on_player_connected(lua_State *l, player_t* player) {
  assert(l);
  assert(player);

  if (lua_getglobal(l, ON_PLAYER_CONNECTED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_pushlightuserdata(l, player);

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player connected hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * TODO(Chris I)
**/
int lua_hook_on_player_disconnected(lua_State *l, player_t* player) {
  assert(l);
  assert(player);

  if (lua_getglobal(l, ON_PLAYER_DISCONNECTED_HOOK_FUNCTION) != LUA_TFUNCTION) {
    lua_pop(l, 1);

    return 0;
  }

  lua_pushlightuserdata(l, player);

  if (lua_pcall(l, 1, 0, 0) != 0) {
    LOG(ERROR, "Error when calling player disconnected hook [%s]", lua_tostring(l, -1));

    return -1;
  }

  return 0;
}

/**
 * TODO(Chris I)
**/
int lua_hook_on_player_input(lua_State *l, player_t* player, const char* input) {
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

