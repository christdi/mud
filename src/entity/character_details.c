#include "mud/entity/character_details.h"

#include <stdlib.h>

/**
 * 
**/
character_details_t * create_character_details_t() {
	character_details_t * character_details = calloc(1, sizeof * character_details);

	return character_details;
}