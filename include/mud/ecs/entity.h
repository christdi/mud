#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <stdlib.h>

/**
 * Definitions
**/
#define UUID_SIZE 37

/**
 * Typedefs
**/
typedef struct game game_t; /* game.h */

/**
 * Structs
**/
typedef struct entity {
  char uuid[UUID_SIZE];
  char* name;
  char* description;
} entity_t;

/**
 * Function prototypes
**/
entity_t* create_entity_t();
void free_entity_t(entity_t* entity);

void load_entities(game_t* game);
entity_t* get_entity(game_t* game, char* uuid);

entity_t* new_character(game_t* game, char* name, char* description);
entity_t* new_item(game_t* game, char* name, char* description);
entity_t* new_location(game_t* game, char* name, char* description);

#endif