#ifndef MUD_LUA_SCRIPT_H
#define MUD_LUA_SCRIPT_H

#include "mud/util/muduuid.h"

/**
 * Typedefs
**/
typedef struct lua_State lua_State;
typedef struct hash_table hash_table_t;
typedef struct game game_t;
typedef struct command command_t;
typedef struct player player_t;

/**
 * Structs
**/
typedef enum {
  ALLOW_DB_API = (1 << 0),
  ALLOW_GAME_API = (1 << 1),
  ALLOW_LOG_API = (1 << 2),
  ALLOW_PLAYER_API = (1 << 3)
} permission_t;

typedef struct script {
  mud_uuid_t uuid;
  char* filepath;
  permission_t permission;
  lua_State* state;
} script_t;

script_t* create_script_t();
void free_script_t(script_t* script);
void deallocate_script(void* value);
int script_load(game_t* game, hash_table_t* scripts, const char* uuid, script_t** script_out);
int script_execute(game_t* game, hash_table_t* scripts, script_t* script);
int script_call_command(script_t* script, command_t* command, player_t* player, const char* arguments);

#endif