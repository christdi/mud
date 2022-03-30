#ifndef MUD_LUA_REF_H
#define MUD_LUA_REF_H

typedef struct lua_State lua_State;

/**
 * Structs
**/
typedef struct lua_ref {
  lua_State* state;

  int ref;
} lua_ref_t;

/**
 * Function prototypes
**/
lua_ref_t* lua_new_lua_ref_t(lua_State* state, int ref);
lua_ref_t* lua_init_lua_ref_t(lua_ref_t* reference, lua_State* state, int ref);
void lua_release_lua_ref_t(lua_ref_t* ref);
void lua_free_lua_ref_t(lua_ref_t* ref);
void lua_deallocate_lua_ref_t(void* value);

#endif