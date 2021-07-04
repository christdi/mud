#ifndef _GENERAL_H_
#define _GENERAL_H_

/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;

/**
 * Function prototypes
**/
void quit_command(player_t* player, game_t* game, char* input);

#endif