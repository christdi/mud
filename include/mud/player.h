#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "mud/network/client.h"


/**
 * Defines
**/
#define MAX_USERNAME_SIZE 30
#define MAX_SEND_SIZE 1024


/**
 * Structs
**/
typedef struct player {
	client_t * client;

	char * username;
	char * passwordHash;
} player_t;


/**
 * Function prototypes
**/
player_t * create_player_t();
void free_player_t(player_t * player);

int send_to_player(player_t * player, const char * fmt, ...);

#endif