#ifndef _STATE_H_
#define _STATE_H_

/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct game game_t;

typedef void (*state_func_t)(player_t* player, game_t* game);
typedef void (*state_input_func_t)(player_t* player, game_t* game, char* input);

/**
 * Structs
**/
typedef struct state {
  void* context;
  state_func_t on_enter;
  state_func_t on_exit;
  state_func_t on_tick;
  state_input_func_t on_input;
} state_t;

/**
 * Function prototypes
**/
state_t* create_state_t();
void free_state_t(state_t* const state);

#endif