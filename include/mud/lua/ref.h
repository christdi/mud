#ifndef MUD_LUA_REF_H
#define MUD_LUA_REF_H

typedef struct lua_State lua_State;

/**
 * Function prototypes
**/
void lua_release_ref(lua_State* l, int ref);

#endif