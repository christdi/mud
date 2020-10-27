#ifndef _CHARACTER_DETAILS_H_
#define _CHARACTER_DETAILS_H_

#include "mud/entity/entity.h"

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

#endif