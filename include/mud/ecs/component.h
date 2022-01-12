#ifndef MUD_ECS_COMPONENT_H
#define MUD_ECS_COMPONENT_H

/**
 * Forward declrations
**/
typedef struct hash_table hash_table_t;

/**
 * Structs
**/
typedef struct component {
  hash_table_t* entities;
} component_t;

typedef struct component_data {
  int ref;
} component_data_t;

/**
 * Functions
**/
component_t* create_component_t();
void free_component_t(component_t* component);
void deallocate_component_t(void* value);

component_data_t* create_component_data_t();
void free_component_data_t(component_data_t* component_data);
void deallocate_component_data_t(void* value);


#endif