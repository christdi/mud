#ifndef _LOCATION_H_
#define _LOCATION_H_

#include "mud/ecs/entity.h"

/**
 * Typedefs
**/
typedef struct components components_t;

/**
 * Structs
**/
typedef struct location {
  char uuid[UUID_SIZE];
  char location_uuid[UUID_SIZE];
} location_t;

/**
 * Function prototypes
**/
location_t* create_location_t();
void free_location_t(location_t* location);

int has_location(components_t* components, entity_t* entity);
void register_location(components_t* components, location_t* location);
location_t* unregister_location(components_t* components, entity_t* entity);
location_t* get_location(components_t* components, entity_t* entity);
void describe_location(location_t* location, char* dest, size_t len);

void update_location(game_t* game);

#endif