#ifndef HG_COMPONENTS_H
#define HG_COMPONENTS_H

#include "mud/structure/hash_table.h"
#include "mud/entity/character_details.h"

/**
 * Structs
**/
typedef struct components {
	hash_table_t * character_details;
} components_t;


/**
 * Function prototypes
**/
components_t * create_components_t();
void free_components_t(components_t * components);


#endif