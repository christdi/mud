#ifndef MUD_ACCOUNT_H
#define MUD_ACCOUNT_H

/**
 * Typedefs
**/
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

#endif