#ifndef MUD_LUA_PLAYER_API_H
#define MUD_LUA_PLAYER_API_H

/**
 * Forward declations
 **/
typedef struct lua_State lua_State;
typedef struct game game_t;

/**
 * Function prototypes
 **/
int lua_player_register_api(lua_State* l);

#endif