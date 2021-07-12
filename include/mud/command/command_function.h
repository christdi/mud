#ifndef MUD_COMMAND_COMMAND_FUNCTION_H
#define MUD_COMMAND_COMMAND_FUNCTION_H

#define MAX_COMMAND_FUNCTION_NAME_SIZE 128

/**
 * Typedefs
**/
typedef void (*command_func)(player_t* player, game_t* game, char* input);

/**
 * Structs
**/
typedef struct command_function {
  char name[MAX_COMMAND_FUNCTION_NAME_SIZE];
  command_func func;
} command_function_t;

#endif