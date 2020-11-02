#ifndef _CHARACTER_DETAILS_H_
#define _CHARACTER_DETAILS_H_


#include "mud/ecs/entity.h"


/**
 * Typedefs
**/
typedef struct components components_t; /* ecs/components.h */
typedef struct game game_t; /* game.h */


/**
 * Structs
**/
typedef struct character_details {
	char uuid[UUID_SIZE];
	char * name;
	char * description;
} character_details_t;


/**
 * Funnction prototypes
**/
character_details_t * create_character_details_t();
void free_character_details_t(character_details_t * character_details);

int has_character_details(components_t * components, char * uuid);
void register_character_details(components_t * components, character_details_t * character_details);
character_details_t * unregister_character_details(components_t * components, char * uuid);
character_details_t * get_character_details(components_t * components, char * uuid);

void update_character_details(game_t * game);

#endif