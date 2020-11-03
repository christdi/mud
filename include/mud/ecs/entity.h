#ifndef _ENTITY_H_
#define _ENTITY_H_


#include <stdlib.h>


/**
 * Definitions
**/
#define UUID_SIZE 37


/**
 * Typedefs
**/
typedef struct game game_t; /* game.h */
typedef struct player player_t; /* player.h */
typedef struct action_callback action_callback_t; /* action/action_callback.h */


/**
 * Structs
**/
typedef struct entity {
	char uuid[UUID_SIZE];
	player_t * player;
	action_callback_t * action_callback;
} entity_t;


/**
 * Function prototypes
**/
entity_t * create_entity_t();
void free_entity_t(entity_t * entity);

void load_entities(game_t * game);
entity_t * get_player_entity(game_t * game, player_t * player);
entity_t * get_entity(game_t * game, char * uuid);
void assign_entity(entity_t * entity, player_t * player);


#endif