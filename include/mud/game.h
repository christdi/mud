#ifndef HG_GAME_H
#define HG_GAME_H_

#include "mud/config.h"
#include "mud/network/network.h"
#include <sys/time.h>

struct game {
  int shutdown;
  struct timeval last_tick;

  network_t *network;
};

typedef struct game game_t;

game_t *game_new(void);
void game_free(game_t *game);
int game_run(config_t *config);
int game_tick(game_t *game, const long nanosecondsPerTick);

#endif
