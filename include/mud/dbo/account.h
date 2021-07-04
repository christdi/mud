#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include <openssl/sha.h>

/**
 * Defines
**/
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 30

/**
 * Typedefs
**/
typedef struct game game_t;

/**
 * Structs
**/
typedef struct account {
  char username[USERNAME_SIZE];
  char password_hash[SHA256_DIGEST_LENGTH * 2];
} account_t;

/**
 * Function prototypes
**/
account_t* create_account_t();
void free_account_t(account_t* account);

int account_save(game_t* game, account_t* account);
int account_load(game_t* game, const char* name, account_t* account);
int account_validate(game_t* game, const char* username, const char* password_hash);
int account_exists(game_t* game, const char* username);

#endif