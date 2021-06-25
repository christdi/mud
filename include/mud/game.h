#ifndef _GAME_H_
#define _GAME_H_

#include <sqlite3.h>
#include <sys/time.h>

/**
 * Typedefs
**/
typedef struct config config_t;
typedef struct hash_table hash_table_t;
typedef struct network network_t;
typedef struct components components_t;

/**
 * Structs
**/
typedef struct game {
  unsigned int shutdown;
  struct timeval last_tick;

  sqlite3* database;

  hash_table_t* players;
  hash_table_t* commands;
  hash_table_t* entities;

  network_t* network;
  components_t* components;
} game_t;

/**
 * Function prototypes
**/
game_t* create_game_t(void);
void free_game_t(game_t* game);

int start_game(game_t* game, config_t* config);

#endif
