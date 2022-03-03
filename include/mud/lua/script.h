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
typedef struct state state_t;
typedef struct event event_t;

/**
 * Structs
**/
typedef enum {
  ALLOW_STD_LIB = (1 << 0),
  ALLOW_DB_API = (1 << 1),
  ALLOW_GAME_API = (1 << 2),
  ALLOW_LOG_API = (1 << 3),
  ALLOW_PLAYER_API = (1 << 4),
  ALLOW_SCRIPT_API = (1 << 5),
  ALLOW_COMMAND_API = (1 << 6)
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
void script_set_permission(script_t* script, permission_t flag, int permitted);
int script_has_permission(script_t* script, permission_t flag);
int script_load(game_t* game, const char* uuid, script_t** script_out);
int script_unload(hash_table_t* scripts, const char* uuid);

int script_call_command(script_t* script, command_t* command, player_t* player, const char* arguments);

#endif