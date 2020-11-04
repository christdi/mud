#ifndef _CONTAINER_H_
#define _CONTAINER_H_


#include "mud/ecs/entity.h"


/**
 * Typedefs
**/
typedef struct linked_list linked_list_t; /* data/hash_table/hash_table.h */
typedef struct components components_t; /* ecs/components.h */
typedef struct entity entity_t; /* ecs/entity.h */
typedef struct game game_t; /* game.h */


/**
 * Structs
**/
typedef struct container {
	char uuid[UUID_SIZE];
	linked_list_t * contains;
} container_t;


/**
 * Function prototypes
**/
container_t * create_container_t();
void free_container_t(container_t * container);

int has_container(components_t * components, entity_t * entity);
void register_container(components_t * components, container_t * container);
container_t * unregister_container(components_t * components, entity_t * entity);
container_t * get_container(components_t * components, entity_t * entity);

void update_container(game_t * game);

#endif