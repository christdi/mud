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
typedef struct script_group {
  char* name;
} script_group_t;

typedef struct script_permission {
  char* module;
  char* method;
} script_permission_t;

typedef struct script {
  mud_uuid_t uuid;
  char* filepath;
} script_t;

script_group_t* script_new_script_group_t(const char* name);
void script_free_script_group_t(script_group_t* script_group);
void script_deallocate_script_group_t(void* value);

script_permission_t* script_new_script_permission_t(const char* module, const char* method);
void script_free_script_permission_t(script_permission_t* script_permission);
void script_deallocate_script_permission_t(void* value);

script_t* create_script_t();
void free_script_t(script_t* script);
void deallocate_script(void* value);

int script_load(game_t* game, const char* uuid, script_t** script_out);
int script_run_command_script(game_t* game, const char* uuid, player_t* player, const char* arguments);

#endif