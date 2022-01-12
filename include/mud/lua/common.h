#ifndef MUD_LUA_COMMON_H
#define MUD_LUA_COMMON_H

/**
 * Typedefs
**/
typedef lua_State lua_State;
typedef lua_Debug lua_Debug;
typedef struct game game_t;
typedef struct sqlite3 sqlite3;

/**
 * Function prototypes
**/
int lua_common_initialise_state(lua_State *l, game_t* game);
game_t* lua_common_get_game(lua_State* l);
sqlite3* lua_common_get_database(lua_State* l);
lua_Debug lua_common_get_debug(lua_State* l);
int lua_common_assert_n_arguments(lua_State *l, int n);

#endif