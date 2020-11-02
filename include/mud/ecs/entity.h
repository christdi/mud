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
typedef struct game game_t;


/**
 * Function prototypes
**/
void load_entities(game_t * game);
void generate_entity_uuid(char * destination, size_t size);


#endif