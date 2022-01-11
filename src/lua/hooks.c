#include <assert.h>

#include "lua.h"

#include "mud/data/linked_list.h"
#include "mud/ecs/entity.h"
#include "mud/lua/hooks.h"
#include "mud/log.h"

#define ON_STARTUP_HOOK_FUNCTION "main"
#define ON_ENTITIES_LOADED_HOOK_FUNCTION "entities_loaded"

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
    mlog(ERROR, "lua_hook_on_startup", "Error when calling on startup hook [%s]", lua_tostring(l, -1));

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
    mlog(ERROR, "lua_hook_on_entities_loaded", "Error when calling entities loaded hook [%s]", lua_tostring(l, -1));
    return -1;
  }

  return 0;
}


