#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/account.h"
#include "mud/data/deallocate.h"
#include "mud/data/linked_list.h"
#include "mud/log.h"

/**
 * ALlocates a new instance of an account_t.
 * 
 * Returns the allocated instance.
**/
account_t* account_t_new() {
  account_t* account = calloc(1, sizeof *account);

  account->entities = create_linked_list_t();
  account->entities->deallocator = deallocate;

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
