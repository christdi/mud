#ifndef _GAME_H_
#define _GAME_H_

#include "mud/network/server.h"

struct game {
    int shutdown;

    server_t * server;
};

typedef struct game game_t;

game_t * game_new();
void game_free(game_t * game);
const int game_run();

#endif
