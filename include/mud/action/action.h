#ifndef _ACTION_H_
#define _ACTION_H_

/**
 * Typedefs
**/
typedef struct entity entity_t; /* ecs/entity.h */
typedef struct game game_t; /* game.h */

/**
 * Function prototypes
**/
void speak_action(entity_t* entity, game_t* game, char* what);

#endif