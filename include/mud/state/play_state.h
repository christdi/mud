#ifndef _PLAY_STATE_H_
#define _PLAY_STATE_H_

/**
 * Defines
**/
#define IDLE_NAG_TIME_SECONDS 10

/**
 * Typedefs
**/
typedef struct player player_t; /* player.h */
typedef struct game game_t; /* game.h */
typedef struct state state_t; /* state.h */

/**
 * Function prototypes
**/
state_t* play_state();

#endif