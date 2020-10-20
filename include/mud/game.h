#ifndef HG_GAME_H
#define HG_GAME_H

#include "mud/config.h"
#include "mud/network/network.h"
#include <sys/time.h>

/**
 * Structs
**/
typedef struct game {
  int shutdown;
  struct timeval lastTick;

  network_t * network;
} game_t;


/**
 * Function prototypes
**/
game_t * game_new(void);
void game_free(game_t * game);

int start_game(config_t * config);

#endif
