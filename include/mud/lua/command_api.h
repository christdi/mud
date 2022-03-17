#ifndef MUD_LUA_COMMAND_API_H
#define MUD_LUA_COMMAND_API_H

/**
 * Forward declations
 **/
typedef struct lua_State lua_State;

/**
 * Function prototypes
 **/
int lua_command_register_api(lua_State* l);

#endif