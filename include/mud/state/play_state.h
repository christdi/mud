#ifndef _PLAY_STATE_H_
#define _PLAY_STATE_H_


/**
 * Typedefs
**/
typedef struct player player_t; /* player.h */
typedef struct game game_t; /* game.h */


/**
 * Function prototypes
**/
void play_state(player_t * player, game_t * game, char * input);


#endif