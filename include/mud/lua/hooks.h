#ifndef MUD_LUA_HOOKS_H
#define MUD_LUA_HOOKS_H

/**
 * Typedefs
**/
typedef struct lua_State lua_State;
typedef struct linked_list linked_list_t;

/**
 * Functions
**/
int lua_hook_on_startup(lua_State *l);
int lua_hook_on_entities_loaded(lua_State *l, linked_list_t* entities);

#endif