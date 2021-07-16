#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/account.h"
#include "mud/data/linked_list.h"
#include "mud/dbo/account_dbo.h"

/**
 * ALlocates a new instance of an account_t.
 * 
 * Returns the allocated instance.
**/
account_t* account_t_new() {
  account_t* account = calloc(1, sizeof *account);

  account->entities = create_linked_list_t();

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

  if (account->entities != NULL) {
    free_linked_list_t(account->entities);
  }

  free(account);
}

/**
 * Populates an account_t with the values from an account_dbo_t.  If any of the fields
 * have already been allocated they will be freed and replaced with the value from the
 * account_dbo_t.
 *
 * Parameters
 *  account - the account that will be populated with the values
 *  account_dbo - the account_dbo that will be used to populate the account
**/
void account_populate_from_account_dbo(account_t* account, account_dbo_t* account_dbo) {
  assert(account);
  assert(account_dbo);

  if (account->username == NULL) {
    free(account->username);
  }

  if (account->password_hash == NULL) {
    free(account->password_hash);
  }

  if (account_dbo->username != NULL) {
    account->username = strdup(account_dbo->username);
  }

  if (account_dbo->password_hash != NULL) {
    account->password_hash = strdup(account_dbo->password_hash);
  }
}

/**
 * Populates an account_t with the data from an account_entity_dbo_t.
 *
 * Parameters
 *  account - the account that will be populated with the values
 *  account_entity_dbo - the account_entity_dbo where the values will come from
**/
void account_populate_from_account_entity_dbo(account_t* account, linked_list_t* account_entities) {
  assert(account);
  assert(account_entities);
}
