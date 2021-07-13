#ifndef _GAME_H_
#define _GAME_H_

#include <sqlite3.h>
#include <sys/time.h>

#define ONE_SECOND_IN_NANOSECONDS 1000000000L
#define ONE_SECOND_IN_MICROSECONDS 100000L

/**
 * Typedefs
**/
typedef struct config config_t;
typedef struct hash_table hash_table_t;
typedef struct linked_list linked_list_t;
typedef struct network network_t;
typedef struct components components_t;
typedef struct narrator narrator_t;

/**
 * Structs
**/
typedef struct game {
  unsigned int shutdown;
  struct timeval last_tick;

  sqlite3* database;

  hash_table_t* players;
  hash_table_t* entities;

  linked_list_t* tasks;
  linked_list_t* events;

  network_t* network;
  components_t* components;
  narrator_t* narrator;
} game_t;

/**
 * Function prototypes
**/
game_t* create_game_t(void);
void free_game_t(game_t* game);

int start_game(config_t* config);

#endif
