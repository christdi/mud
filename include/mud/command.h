#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "mud/util/muduuid.h"
/**
 * Typedefs
 **/
typedef struct player player_t;
typedef struct game game_t;

/**
 * Structs
 **/
typedef struct command {
  mud_uuid_t uuid;
  char* name;
  mud_uuid_t script;
} command_t;

/**
 * Function prototypes
 **/
command_t* command_new_command_t(const char* uuid, const char* name, const char* script_uuid);
void command_free_command_t(command_t* command);
void command_deallocate_command_t(void* value);

int command_load_commands(game_t* game);

#endif