#ifndef MUD_LUA_DB_API_H
#define MUD_LUA_DB_API_H

/**
 * Forward declations
 **/
typedef struct lua_State lua_State;
typedef struct sqlite3 sqlite3;

/**
 * Function prototypes
 **/
int lua_db_register_api(lua_State* l);

#endif