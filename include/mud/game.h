#ifndef _GAME_H_
#define _GAME_H_

#include "mud/network/network.h"

struct game {
    int shutdown;

    network_t * network;
};

typedef struct game game_t;

game_t * game_new();
void game_free(game_t * game);
const int game_run();

#endif
