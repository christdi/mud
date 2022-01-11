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
  char* function;
  mud_uuid_t script;
} command_t;

/**
 * Function prototypes
**/
command_t* create_command_t();
void free_command_t(command_t* command);
void deallocate_command(void* value);

void load_commands(game_t* game);

int execute_command(game_t* game, player_t* player, const char* command, const char* arguments);

#endif