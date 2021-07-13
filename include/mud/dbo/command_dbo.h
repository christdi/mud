#ifndef MUD_DBO_COMMAND_DBO_H
#define MUD_DBO_COMMAND_DBO_H

#define COMMAND_NAME_LENGTH 128
#define COMMAND_FUNCTION_LENGTH 128

/**
 * Typedefs
**/
typedef struct game game_t;
typedef struct linked_list linked_list_t;

/**
 * Structs
**/
typedef struct command_dbo {
  char name[COMMAND_NAME_LENGTH];
  char function[COMMAND_FUNCTION_LENGTH];
} command_dbo_t;

/**
 * Function prototypes
**/
command_dbo_t* create_command_dbo_t();
void free_command_dbo_t(command_dbo_t* command_dbo);
void deallocate_command_dbo_t(void* value);

int select_commands_by_name(game_t* game, const char* name, linked_list_t* commands);

#endif