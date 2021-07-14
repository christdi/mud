#include <assert.h>
#include <stdlib.h>

#include "mud/account.h"
#include "mud/dbo/account_dbo.h"

/**
 * ALlocates a new instance of an account_t.
 * 
 * Returns the allocated instance.
**/
account_t* account_t_new() {
  account_t* account = calloc(1, sizeof account_t);

  return account;
}

/**
 * Frees an allocated instance of an account_t.
 * 
 * Parameters
 *  account - the allocated account_t to be freed.
**/
void account_t_free(account_t* account) {
  assert(account);

  if (account->username != NULL) {
    free(account->username);
  }

  if (account->password_hash != NULL) {
    free(account->password_hash);
  }

  free(account);
}

/**
 * Creates a new account instance from a populated account_dbo instance.
 * 
 * Parameters
 *  account_dbo - the account_dbo to create an account_t from.
 * 
 * Returns an allocated instance of an account_t populated with the details from
 * the account_dbo.  It's the callers responsibility to free the account..  
**/
account_t* account_t_from_account_dbo_t(account_dbo_t* account_dbo) {
  account_t* account = account_t_new();

  

  return account;
}
