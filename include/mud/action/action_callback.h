#ifndef _ACTION_CALLBACK_H_
#define _ACTION_CALLBACK_H_

/**
 * Typedefs
**/
typedef struct game game_t; /* game.h */
typedef struct entity entity_t; /* ecs/entity.h */

typedef void (*speak_func)(game_t* game, entity_t* entity, char* what);

/**
 * Structs
**/
typedef struct action_callback {
  speak_func on_speak;
} action_callback_t;

/**
 * Function prototypes
**/
action_callback_t* create_action_callback_t();
void free_action_callback_t(action_callback_t* action_callback_t);

#endif