#ifndef _GAME_H_
#define _GAME_H_

#include "mud/config.h"
#include "mud/network/network.h"
#include "mud/entity/components.h"
#include "mud/structure/list.h"
#include "mud/structure/hash_table.h"

#include <sys/time.h>

/**
 * Structs
**/
typedef struct game {
  unsigned int shutdown;
  struct timeval last_tick;

  hash_table_t * players;
  network_t * network;
  components_t * components;
} game_t;


/**
 * Function prototypes
**/
game_t * create_game_t(void);
void free_game_t(game_t * game);

int start_game(game_t * game, config_t * config);

#endif
