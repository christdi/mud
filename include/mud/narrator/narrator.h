#ifndef _NARRATOR_H_
#define _NARRATOR_H_


/**
 * Typedefs
**/
typedef struct entity entity_t;
typedef struct game game_t;


/**
 * Function prototypes
**/
void narrate_on_speak(game_t * game, entity_t * entity, char * what);


#endif