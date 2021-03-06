#ifndef _STATE_H_
#define _STATE_H_


/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;


typedef void (*state_func_t)(player_t * player, game_t * game, char * input);



#endif