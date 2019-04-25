#ifndef _GAME_H_
#define _GAME_H_

#include <sys/time.h>
#include "mud/network/network.h"

struct game {
    int shutdown;
    struct timeval last_tick;

    network_t * network;
};

typedef struct game game_t;

game_t * game_new();
void game_free(game_t * game);
const int game_run();
const int game_tick(game_t * game);

#endif
