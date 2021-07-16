#ifndef _COMMAND_H_
#define _COMMAND_H_

/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;

typedef void (*command_func_t)(player_t* player, game_t* game, char* input);

/**
 * Structs
**/
typedef struct command {
  char* name;
  command_func_t func;
} command_t;

/**
 * Function prototypes
**/
command_t* create_command_t();
void free_command_t(command_t* command);
void deallocate_command(void* value);

void load_commands(game_t* game);
const command_t* get_command(game_t* game, const char* name);

#endif