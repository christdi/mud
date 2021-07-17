#ifndef MUD_ACCOUNT_H
#define MUD_ACCOUNT_H

/**
 * Typedefs
**/
typedef struct account_dbo account_dbo_t;
typedef struct account_entity_dbo account_entity_dbo_t;
typedef struct linked_list linked_list_t;

/**
 * Structs
**/
typedef struct account {
  char* username;
  char* password_hash;
  linked_list_t* entities;
} account_t;

/**
 * Function prototypes
**/
account_t* account_t_new();
void account_t_free(account_t* account);

void account_populate_from_account_dbo(account_t* account, account_dbo_t* account_dbo);
void account_populate_from_account_entity_dbo(account_t* account, linked_list_t* account_entities);

#endif