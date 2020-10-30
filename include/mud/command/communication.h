#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_


/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;


/**
 * Function prototypes
**/
void say_command(player_t * player, game_t * game, char * input);


#endif