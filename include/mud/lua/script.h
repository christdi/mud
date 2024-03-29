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
typedef struct event event_t;
typedef struct entity entity_t;
typedef struct lua_ref lua_ref_t;

/**
 * Structs
 **/
typedef struct script_group {
  mud_uuid_t uuid;
  char* filepath;
  char* name;
  char* description;
} script_group_t;

typedef struct script {
  mud_uuid_t uuid;
  char* filepath;
} script_t;

script_group_t* script_new_script_group_t(const char* uuid, const char*filepath, const char* name, const char* description);
void script_free_script_group_t(script_group_t* script_group);
void script_deallocate_script_group_t(void* value);

script_t* create_script_t();
void free_script_t(script_t* script);
void deallocate_script(void* value);

int script_run_command_script(game_t* game, const char* uuid, player_t* player, const char* arguments);
int script_run_action_script(game_t* game, const char* uuid, entity_t* entity, lua_ref_t* ref);

#endif