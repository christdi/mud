#ifndef MUD_ECS_ARCHETYPE_H
#define MUD_ECS_ARCHETYPE_H

#include <stdbool.h>

/**
 * Typedefs
 **/
typedef struct linked_list linked_list_t;
typedef struct hash_table hash_table_t;
typedef struct component component_t;
typedef struct entity entity_t;

/**
 * Structs
 **/
typedef struct archetype {
  hash_table_t* entities;
  linked_list_t* components;
} archetype_t;

/**
 * Function prototypes
 **/
archetype_t* ecs_new_archetype_t();
void ecs_free_archetype_t(archetype_t* archetype);
void ecs_deallocate_archetype_t(void* value);

void ecs_add_archetype_component(archetype_t* archetype, component_t* component);
void ecs_remove_archetype_component(archetype_t* archetype, component_t* component);
bool ecs_entity_matches_archetype(archetype_t* archetype, entity_t* entity);
void ecs_add_entity_to_archetype(archetype_t* archetype, entity_t* entity);
void ecs_remove_entity_from_archetype(archetype_t* archetype, entity_t* entity);
bool ecs_archetype_has_entity(archetype_t* archetype, entity_t* entity);
void ecs_update_entity_archetypes(linked_list_t* archetypes, entity_t* entity);

#endif