#ifndef MUD_DBO_ACCOUNT_DBO_H
#define MUD_DBO_ACCOUNT_DBO_H

#include "mud/util/mudhash.h"

/**
 * Defines
**/
#define ACCOUNT_DBO_USERNAME_SIZE 30

/**
 * Typedefs
**/
typedef struct game game_t;

/**
 * Structs
**/
typedef struct account_dbo {
  char* username;
  char password_hash[SHA256_HEX_SIZE];
} account_dbo_t;

/**
 * Function prototypes
**/
account_dbo_t* create_account_dbo_t();
void free_account_dbo_t(account_dbo_t* account);

int account_dbo_save(game_t* game, account_dbo_t* account);
int account_dbo_validate(game_t* game, const char* username, const char* password_hash);
int account_dbo_exists(game_t* game, const char* username);

#endif