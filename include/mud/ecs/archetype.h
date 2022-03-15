#ifndef MUD_ECS_ARCHETYPE_H
#define MUD_ECS_ARCHETYPE_H

/**
 * Typedefs
**/
typedef struct linked_list linked_list_t;
typedef struct hash_table hash_table_t;
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
archetype_t* archetype_new_archetype_t();
void archetype_free_archetype_t(archetype_t* archetype);
void archetype_deallocate_archetype_t(void* value);

void archetype_entity_matches(archetype_t* archetype, entity_t* entity);
void archetype_add_entity(archetype_t* archetype, entity_t* entity);
void archetype_remove_entity(archetype_t* archetype, entity_t* entity);

#endif