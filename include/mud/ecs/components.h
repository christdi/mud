#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_


/**
 * Typedefs
**/
typedef struct hash_table hash_table_t; /* data/hash_table/hash_table.h */
typedef struct container container_t; /* ecs/container.h */
typedef struct contained contained_t; /* ecs/container.h */
typedef struct description description_t; /* ecs/description.h */



/**
 * Structs
**/
typedef struct components {
	hash_table_t * description;
	hash_table_t * container;
	hash_table_t * contained;
} components_t;


/**
 * Function prototypes
**/
components_t * create_components_t();
void free_components_t(components_t * components);

#endif