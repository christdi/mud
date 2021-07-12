#ifndef _DESCRIPTION_H_
#define _DESCRIPTION_H_

#include "mud/ecs/entity.h"
#include "mud/ecs/entity_id.h"

/**
 * Typedefs
**/
typedef struct components components_t; /* ecs/components.h */
typedef struct entity entity_t; /* ecs/entity.h */
typedef struct game game_t; /* game.h */

/**
 * Structs
**/
typedef struct description {
  entity_id_t entity_id;
  
  char* name;
  char* description;
} description_t;

/**
 * Funnction prototypes
**/
description_t* create_description_t();
void free_description_t(description_t* description);
void deallocate_description_t(void *value);

int has_description(components_t* components, entity_t* entity);
void register_description(components_t* components, description_t* description);
void unregister_description(components_t* components, entity_t* entity);
description_t* get_description(components_t* components, entity_t* entity);

void update_description(game_t* game);

#endif