#include <assert.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "mud/account.h"
#include "mud/data/linked_list.h"
#include "mud/dbo/account_dbo.h"
#include "mud/game.h"
#include "mud/log.h"

/**
 * Creates and returns an initialised account_t struct.
**/
account_dbo_t* account_dbo_t_new() {
  account_dbo_t* account = calloc(1, sizeof *account);

  account->username = NULL;
  account->password_hash = NULL;

  return account;
}

/**
 * Frees an initialised account_t struct and members.
**/
void account_dbo_t_free(account_dbo_t* account) {
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
 * Allocates a new instance of account_entity_dbo_t.
 * 
 * Returns the newly allocated instance.
**/
account_entity_dbo_t* account_entity_dbo_t_new() {
  account_entity_dbo_t* account_entity_dbo = calloc(1, sizeof *account_entity_dbo);

  return account_entity_dbo;
}

/**
 * Frees an allocated account_enttiy_dbo_t.
 * 
 * Parameters
 *  account_entity_dbo - the account_entity_dbo_t to be freed
**/
void account_entity_dbo_t_free(account_entity_dbo_t* account_entity_dbo) {
  assert(account_entity_dbo);

  if (account_entity_dbo->account_username != NULL) {
    free(account_entity_dbo->account_username);
  }

  if (account_entity_dbo->entity_uuid != NULL) {
    free(account_entity_dbo->entity_uuid);
  }

  free(account_entity_dbo);
}

/**
 * Deallocator for account_entity_dbo_t for use in data structures.
 * 
 * Parameters
 *  value - a void* that must be an entity_account_dbo_t* or behaviour is undefined
**/
void account_entity_dbo_t_deallocate(void* value) {
  assert(value);

  account_entity_dbo_t_free((account_entity_dbo_t*)value);
}

/**
**/
int account_dbo_save(game_t* game, account_dbo_t* account) {
  sqlite3_stmt* res = NULL;

  const char* sql = "INSERT INTO account(username, password_hash) VALUES(?, ?) "
                    "ON CONFLICT(username) DO UPDATE SET username = excluded.username, password_hash = excluded.password_hash";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "account_save", "Failed to prepare statement to insert account into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, account->username, (int)strlen(account->username), NULL) != SQLITE_OK) {
    mlog(ERROR, "account_save", "Failed to bind username to insert account into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, account->password_hash, (int)strlen(account->password_hash), NULL) != SQLITE_OK) {
    mlog(ERROR, "account_save", "Failed to bind password hash to insert account into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  };

  if (sqlite3_step(res) != SQLITE_DONE) {
    mlog(ERROR, "account_save", "Failed to insert account into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}

int account_dbo_get_by_name(game_t* game, const char* username, account_dbo_t* result) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT username, password_hash FROM account WHERE username=?";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "account_dbo_load", "Failed to prepare statement to load account from database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    mlog(ERROR, "account_dbo_load", "Failed to bind username to retrieve account from database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    mlog(ERROR, "account_dbo_load", "Failed to retreive account from database: [%s]", sqlite3_errmsg(game->database));

    sqlite3_finalize(res);

    return -1;
  }

  if (result->username != NULL) {
    free(result->username);
  }

  if (result->password_hash != NULL) {
    free(result->password_hash);
  }

  result->username = strdup((char*)sqlite3_column_text(res, 0));
  result->password_hash = strdup((char*)sqlite3_column_text(res, 1));

  sqlite3_finalize(res);

  return 0;
}

int account_dbo_exists(game_t* game, const char* username) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT EXISTS(SELECT 1 FROM account WHERE username=?)";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "account_exists", "Failed to prepare statement to validate account in database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    mlog(ERROR, "account_exists", "Failed to bind username to validate account in database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_step(res) != SQLITE_ROW) {
    mlog(ERROR, "account_exists", "Failed to retrieve any rows to validate account in database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  int exists = sqlite3_column_int(res, 0);

  sqlite3_finalize(res);

  return exists == 1 ? 0 : -1;
}

/**
 * Populates an account_dbo_t with the values from an account_t.  If any of the fields
 * have already been allocated they will be freed and replaced with the value from the
 * account_t.
 *
 * Parameters
 *  account_dbo - the account_dbo that will be populated with the values
 *  account - the account that will be used to populate the account_dbo_t
**/
void account_dbo_populate_from_account(account_dbo_t* account_dbo, account_t* account) {
  if (account_dbo->username == NULL) {
    free(account_dbo->username);
  }

  if (account_dbo->password_hash == NULL) {
    free(account_dbo->password_hash);
  }

  if (account->username != NULL) {
    account_dbo->username = strdup(account->username);
  }

  if (account->password_hash != NULL) {
    account_dbo->password_hash = strdup(account->password_hash);
  }
}

/**
**/
int account_entity_dbo_get_by_username(game_t* game, const char* username, linked_list_t* results) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT account_username, entity_uuid FROM account_entity WHERE account_username=?";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "account_entity_dbo_get_by_username", "Failed to prepare statement to load account entities from database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    mlog(ERROR, "account_entity_dbo_get_by_username", "Failed to bind username to retrieve account entities from database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      mlog(ERROR, "account_entity_dbo_get_by_username", "Failed to retreive account entities from database: [%s]", sqlite3_errmsg(game->database));

      sqlite3_finalize(res);

      return 0;
    }

    account_entity_dbo_t* account_entity_dbo = account_entity_dbo_t_new();
    account_entity_dbo->account_username = strdup((char*)sqlite3_column_text(res, 0));
    account_entity_dbo->entity_uuid = strdup((char*)sqlite3_column_text(res, 1));

    list_add(results, (account_entity_dbo_t*) account_entity_dbo);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}
