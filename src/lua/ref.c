#include <assert.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"

#include "mud/log.h"
#include "mud/lua/ref.h"

/**
 * Allocates and returns a new Lua ref for a given state and ref.
 *
 * state - the state the reference comes from
 * ref - the actual reference
 *
 * Returns the newly allocated instance of lua_ref_t
**/
lua_ref_t* lua_new_lua_ref_t(lua_State* state, int ref) {
  assert(state);

  lua_ref_t* reference = lua_init_lua_ref_t(calloc(1, sizeof(lua_ref_t)), state, ref);

  return reference;
}

/**
 * Initialises an instance of lua_ref_t.
 *
 * reference - the lua_ref_t instance to be initialised
 * state - the state that the reference is associated with
 * ref - the actual luaL_ref reference
 *
 * Returns the newly initialised instance of lua_ref_t*
**/
lua_ref_t* lua_init_lua_ref_t(lua_ref_t* reference, lua_State* state, int ref) {
  assert(reference);
  assert(state);

  reference->state = state;
  reference->ref = ref;

  return reference;
}

/**
 * Releases the reference of an instance of lua_ref_t
 *
 * ref - the lua_ref_t to release the reference of./
**/
void lua_release_lua_ref_t(lua_ref_t* ref) {
  luaL_unref(ref->state, LUA_REGISTRYINDEX, ref->ref);
}

/**
 * Frees an allocated lua_ref_t.
 *
 * ref - the lua_ref_t to free
**/
void lua_free_lua_ref_t(lua_ref_t* ref) {
  assert(ref);

  if (ref->state != NULL) {
    lua_release_lua_ref_t(ref);
  }

  free(ref);
}

/**
 * Deallocates a void pointer to lua_ref_t
 *
 * value - void pointer to lua_ref_t
**/
void lua_deallocate_lua_ref_t(void* value) {
  assert(value);

  lua_free_lua_ref_t(value);

}
