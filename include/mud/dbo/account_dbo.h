#ifndef MUD_DBO_ACCOUNT_DBO_H
#define MUD_DBO_ACCOUNT_DBO_H

#include <openssl/sha.h>

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
  char username[ACCOUNT_DBO_USERNAME_SIZE];
  char password_hash[SHA256_DIGEST_LENGTH * 2];
} account_dbo_t;

/**
 * Function prototypes
**/
account_dbo_t* create_account_dbo_t();
void free_account_dbo_t(account_dbo_t* account);

int account_save(game_t* game, account_dbo_t* account);
int account_validate(game_t* game, const char* username, const char* password_hash);
int account_exists(game_t* game, const char* username);

#endif