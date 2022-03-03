#ifndef _GAME_H_
#define _GAME_H_

#include <sqlite3.h>
#include <sys/time.h>

#define ONE_SECOND_IN_NANOSECONDS 1000000000L
#define ONE_SECOND_IN_MICROSECONDS 100000L
#define GAME_PLAYER_PULSE_SECONDS 10

/**
 * Typedefs
**/
typedef struct config config_t;
typedef struct hash_table hash_table_t;
typedef struct linked_list linked_list_t;
typedef struct network network_t;
typedef struct event_broker event_broker_t;
typedef struct script_repository script_repository_t;
typedef struct lua_State lua_State;

/**
 * Structs
**/
typedef struct game {
  unsigned int shutdown;
  struct timeval last_tick;

  config_t* config;

  sqlite3* database;

  hash_table_t* templates;
  hash_table_t* players;
  hash_table_t* entities;

  script_repository_t* scripts;
  event_broker_t* event_broker;

  linked_list_t* components;
  linked_list_t* tasks;
  linked_list_t* events;

  network_t* network;
  lua_State* lua_state;
} game_t;

/**
 * Function prototypes
**/
game_t* create_game_t(void);
void free_game_t(game_t* game);

int start_game(int argc, char* argv[]);

#endif
