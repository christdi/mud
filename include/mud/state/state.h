#ifndef _STATE_H_
#define _STATE_H_

#include <mud/util/muduuid.h>

/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;
typedef struct mud_uuid mud_uuid_t;

/**
 * Structs
**/
typedef struct state {
  int ref;
} state_t;

/**
 * Function prototypes
**/
state_t* create_state_t();
void free_state_t(state_t* state);

#endif