#ifndef _LOGIN_STATE_H_
#define _LOGIN_STATE_H_

/**
 * Typedefs
**/
typedef struct player player_t; /* player.h */
typedef struct game game_t; /* game.h */

/**
 * Function prototypes
**/
void login_state(player_t* player, game_t* game, char* input);

#endif