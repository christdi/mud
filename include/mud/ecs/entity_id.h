#ifndef MUD_ECS_ENTITY_ID_H
#define MUD_ECS_ENTITY_ID_H

#include "mud/util/muduuid.h"

/**
 * Structs
**/
typedef struct entity_id {
  char uuid[UUID_SIZE];
} entity_id_t;

entity_id_t entity_id();

#endif