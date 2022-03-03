#include <assert.h>
#include <stdlib.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/lua/event.h"

/**
 * Allocates a new instance of lua_event_data_t on the heap.
 *
 * Returns the allocated instance
**/
lua_event_data_t* lua_new_lua_event_data_t(lua_State* state, int ref) {
  assert(state);

  lua_event_data_t* lua_event_data = calloc(1, sizeof *lua_event_data);

  lua_event_data->state = state;
  lua_event_data->ref = ref;

  return lua_event_data;
}

/**
 * Frees an allocated instance of lua_event_data_t.
 *
 * Parameters
 *   lua_event_data - the lua_event_data_t to be freed.
**/
void lua_free_lua_event_data_t(lua_event_data_t* lua_event_data) {
  assert(lua_event_data);

  luaL_unref(lua_event_data->state, LUA_REGISTRYINDEX, lua_event_data->ref);

  lua_event_data->state = NULL;
  lua_event_data->ref = LUA_REFNIL;

  free(lua_event_data);
}

/**
 * Deallocator for use with lua_event_data-t wrapped in event or collection.
 *
 * Parameters
 *   lua_event_data - void pointer to lua_event_data_t
**/
void lua_deallocate_lua_event_data_t(void* lua_event_data) {
  assert(lua_event_data);

  lua_free_lua_event_data_t(lua_event_data);
}
