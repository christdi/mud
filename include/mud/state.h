#ifndef _STATE_H_
#define _STATE_H_

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