#ifndef MUD_ACCOUNT_H
#define MUD_ACCOUNT_H

/**
 * Typedefs
**/
typedef struct account_dbo account_dbo_t;

/**
 * Structs
**/
typedef struct account {
	char* username;
	char* password_hash;
} account_t;

/**
 * Function prototypes
**/
account_t* account_t_new();
void account_t_free(account_t* account);

account_t* account_t_from_account_dbo_t(account_dbo_t* account_dbo);

#endif