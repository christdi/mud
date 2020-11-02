#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "mud/network/client.h"
#include "mud/state/state.h"
#include "mud/ecs/entity.h"


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


/**
 * Structs
**/
typedef struct player {
	client_t * client;
	state_func_t state;

	char username[USERNAME_SIZE];
	char password_hash[PASSWORD_SIZE];
	char entity[UUID_SIZE];
} player_t;


/**
 * Function prototypes
**/
player_t * create_player_t();
void free_player_t(player_t * player);


void player_connected(client_t * client, void * context);
void player_disconnected(client_t * client, void * context);
void player_input(client_t * client, void * context);


void send_to_player(player_t * player, const char * fmt, ...);
void send_to_all_players(game_t * game, player_t * excluding, const char * fmt, ...);

#endif