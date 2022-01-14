#ifndef MUD_LUA_HOOKS_H
#define MUD_LUA_HOOKS_H

/**
 * Typedefs
**/
typedef struct lua_State lua_State;
typedef struct linked_list linked_list_t;
typedef struct player player_t;

/**
 * Functions
**/
int lua_hook_on_startup(lua_State* l);
int lua_hook_on_entities_loaded(lua_State* l, linked_list_t* entities);
int lua_hook_on_player_connected(lua_State* l, player_t* player);
int lua_hook_on_player_disconnected(lua_State* l, player_t* player);
int lua_hook_on_player_input(lua_State* l, player_t* player, const char* input);

#endif