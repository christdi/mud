#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_


/**
 * Typedefs
**/
typedef struct hash_table hash_table_t; /* data/hash_table/hash_table.h */
typedef struct character_details character_details_t; /* data/entity/character_details.h */


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