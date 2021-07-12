#ifndef _COMMAND_H_
#define _COMMAND_H_

/**
 * Defines
**/
#define ARGUMENT_SIZE 256
#define NAME_SIZE 20

/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;

typedef void (*command_func)(player_t* player, game_t* game, char* input);

/**
 * Structs
**/
typedef struct command {
  char name[NAME_SIZE];
  command_func func;
} command_t;

/**
 * Function prototypes
**/
command_t* create_command_t();
void free_command_t(command_t* command);
void deallocate_command(void* value);

void load_commands(game_t* game);
command_t* get_command(game_t* game, char* name);

#endif