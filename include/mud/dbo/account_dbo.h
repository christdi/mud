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
typedef struct linked_list linked_list_t;

/**
 * Structs
**/
typedef struct account_dbo {
  char* username;
  char* password_hash;
} account_dbo_t;

typedef struct account_entity_dbo {
  char* account_username;
  char* entity_uuid;
} account_entity_dbo_t;

/**
 * Function prototypes
**/
account_dbo_t* account_dbo_t_new();
void account_dbo_t_free(account_dbo_t* account);

account_entity_dbo_t* account_entity_dbo_t_new();
void account_entity_dbo_t_free(account_entity_dbo_t* account_entity_dbo);
void account_entity_dbo_t_deallocate(void* value);

int account_dbo_save(game_t* game, account_dbo_t* account);
int account_dbo_get_by_name(game_t* game, const char* username, account_dbo_t* result);
int account_dbo_exists(game_t* game, const char* username);
void account_dbo_populate_from_account(account_dbo_t* account_dbo, account_t* account);

int account_entity_dbo_get_by_username(game_t* game, const char* username, linked_list_t* results);

#endif