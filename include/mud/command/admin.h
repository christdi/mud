#ifndef _ADMIN_H_
#define _ADMIN_H_


/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;


/**
 * Function prototypes
**/
void entity_command(player_t * player, game_t * game, char * input);


#endif