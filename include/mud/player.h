#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "mud/network/client.h"
#include "mud/util/muduuid.h"

/**
 * Defines
**/
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 30
#define SEND_SIZE 1024
#define COMMAND_SIZE 256

/**
 * Typedefs
**/
typedef struct game game_t;
typedef struct entity entity_t;
typedef struct account account_t;
typedef struct state state_t;
typedef struct linked_list linked_list_t;

/**
 * Structs
**/
typedef struct player {
  client_t* client;
  entity_t* entity;
  account_t* account;
  state_t* state;
} player_t;

/**
 * Function prototypes
**/
player_t* create_player_t();
void free_player_t(player_t* player);
void deallocate_player(void* value);

void player_connected(client_t* client, void* context);
void player_disconnected(client_t* client, void* context);
void player_input(client_t* client, void* context);

void player_change_state(player_t* player, game_t* game, state_t* state);
void player_on_tick(player_t *player, game_t* game);

void send_to_player(player_t* player, const char* fmt, ...);
void send_to_players(linked_list_t* players, const char* fmt, ...);
void send_to_all_players(game_t* game, player_t* excluding, const char* fmt, ...);

#endif