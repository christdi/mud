#ifndef _LOCATION_H_
#define _LOCATION_H_

#include "mud/ecs/entity.h"
#include "mud/ecs/entity_id.h"

/**
 * Typedefs
**/
typedef struct components components_t;

/**
 * Structs
**/
typedef struct location {
  entity_id_t entity_id;

  entity_id_t at;
} location_t;

/**
 * Function prototypes
**/
location_t* create_location_t();
void free_location_t(location_t* location);
void deallocate_location_t(void* value);

int has_location(components_t* components, entity_t* entity);
void register_location(components_t* components, location_t* location);
void unregister_location(components_t* components, entity_t* entity);
location_t* get_location(components_t* components, entity_t* entity);
void describe_location(location_t* location, char* dest, size_t len);

#endif