#ifndef MUD_LUA_EVENT_H
#define MUD_LUA_EVENT_H

/**
 * Typedefs
**/
typedef struct lua_State lua_State;

/**
 * Structs
**/
typedef struct lua_event_data {
  lua_State* state;
  int ref;
} lua_event_data_t;

/**
 * Function prototypes
**/
lua_event_data_t* lua_new_lua_event_data_t(lua_State* state, int ref);
void lua_free_lua_event_data_t(lua_event_data_t* lua_event_data);
void lua_deallocate_lua_event_data_t(void* lua_event_data);

#endif