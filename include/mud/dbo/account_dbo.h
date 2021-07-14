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
typedef struct account account_t;

/**
 * Structs
**/
typedef struct account_dbo {
  char* username;
  char* password_hash;
} account_dbo_t;

/**
 * Function prototypes
**/
account_dbo_t* account_dbo_t_new();
void account_dbo_t_free(account_dbo_t* account);

int account_dbo_save(game_t* game, account_dbo_t* account);
account_dbo_t* account_dbo_get_by_name(game_t* game, const char* username);
int account_dbo_validate(game_t* game, const char* username, const char* password_hash);
int account_dbo_exists(game_t* game, const char* username);
void account_dbo_from_account(account_dbo_t* account_dbo, account_t* account);

#endif