#ifndef _EXPLORE_H_
#define _EXPLORE_H_


/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;


/**
 * Function prototypes
**/
void inventory_command(player_t * player, game_t * game, char * input);
void look_command(player_t * player, game_t * game, char * input);


#endif