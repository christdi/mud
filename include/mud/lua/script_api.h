#ifndef MUD_LUA_SCRIPT_API_H
#define MUD_LUA_SCRIPT_API_H

/**
 * Forward declations
 **/
typedef struct lua_State lua_State;
typedef struct game game_t;

/**
 * Function prototypes
 **/
int lua_script_register_api(lua_State* l);

#endif