#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdbool.h>

#include "mud/util/muduuid.h"
/**
 * Typedefs
 **/
typedef struct player player_t;
typedef struct game game_t;
typedef struct linked_list linked_list_t;
typedef struct hash_table hash_table_t;

/**
 * Structs
 **/
typedef struct command {
  mud_uuid_t uuid;
  char* name;
  mud_uuid_t script;
} command_t;

typedef struct command_group {
  mud_uuid_t uuid;
  char* description;
  linked_list_t* commands; // contains command uuids
} command_group_t;

/**
 * Function prototypes
 **/
command_t* command_new_command_t(const char* uuid, const char* name, const char* script_uuid);
void command_free_command_t(command_t* command);
void command_deallocate_command_t(void* value);

command_group_t* command_new_command_group_t(const char* uuid, const char* description);
void command_free_command_group_t(command_group_t* group);
void command_deallocate_command_group_t(void* value);

int command_load_commands(game_t* game);
int command_load_command_groups(game_t* game);

command_group_t* command_get_command_group_by_id(game_t* game, const char* uuid);

#endif