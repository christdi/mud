#ifndef _CHARACTER_H_
#define _CHARACTER_H_


/**
 * Typedefs
**/
typedef struct entity entity_t;
typedef struct game game_t;


/**
 * Function prototypes
**/
entity_t * new_character(game_t * game, char * name, char * description);


#endif