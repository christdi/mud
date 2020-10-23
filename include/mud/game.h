#ifndef HG_GAME_H
#define HG_GAME_H

#include "mud/config.h"
#include "mud/network/network.h"
#include "mud/entity/components.h"
#include "mud/structure/list.h"

#include <sys/time.h>

/**
 * Structs
**/
typedef struct game {
  int shutdown;
  struct timeval last_tick;

  network_t * network;
  components_t * components;
  list_t * events;
} game_t;


/**
 * Function prototypes
**/
game_t * create_game_t(void);
void free_game_t(game_t * game);

int start_game(game_t * game, config_t * config);

#endif
