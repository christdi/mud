#ifndef MUD_LUA_API_H
#define MUD_LUA_API_H

/**
 * Forward declations
**/
typedef struct lua_State lua_State;

/**
 * Function prototypes
**/
int lua_register_api(lua_State* l);

static int lua_register_component(lua_State* l);
static int lua_has_component(lua_State* l);
static int lua_add_component(lua_State* l);
static int lua_get_component(lua_State* l);

#endif