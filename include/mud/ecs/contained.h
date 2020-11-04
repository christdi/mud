#ifndef _CONTAINED_H_
#define _CONTAINED_H_


#include "mud/ecs/entity.h"


/**
 * Typedefs
**/
typedef struct components components_t; /* ecs/components.h */
typedef struct entity entity_t; /* ecs/entity.h */
typedef struct game game_t; /* game.h */


/**
 * Structs
**/
typedef struct contained {
	char uuid[UUID_SIZE];
	char container[UUID_SIZE];
} contained_t;


/**
 * Function prototypes
**/
contained_t * create_contained_t();
void free_contained_t(contained_t * contained);

int has_contained(components_t * components, entity_t * entity);
void register_contained(components_t * components, contained_t * contained);
contained_t * unregister_contained(components_t * components, entity_t * entity);
contained_t * get_contained(components_t * components, entity_t * entity);

void update_contained(game_t * game);

#endif