#ifndef MUD_LUA_COMMON_H
#define MUD_LUA_COMMON_H

#define PLAYER_DATA_TABLE_NAME "_players"

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
int lua_initialise_state(lua_State* l, game_t* game);

void lua_push_api_table(lua_State* l);
game_t* lua_get_game(lua_State* l);
sqlite3* lua_get_database(lua_State* l);
lua_Debug lua_get_debug(lua_State* l);

void lua_log_stack(lua_State* l);

#endif