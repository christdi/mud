#ifndef _GAME_H_
#define _GAME_H_

struct game {
};

typedef struct game game_t;

const int game_run();
const int game_bind_servers();
const int game_close_servers();

#endif
