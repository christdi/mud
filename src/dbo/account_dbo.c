#include <assert.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "mud/dbo/account_dbo.h"
#include "mud/game.h"
#include "mud/log.h"

/**
 * Creates and returns an initialised account_t struct.
**/
account_t* create_account_dbo_t() {
  account_dbo_t* account = calloc(1, sizeof *account);

  account->username[0] = '\0';
  account->password_hash[0] = '\0';

  return account;
}

/**
 * Frees an initialised account_t struct and members.
**/
void free_account_dbo_t(account_t* account) {
  assert(account);

  free(account);
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

/**
**/
int account_dbo_validate(game_t* game, const char* username, const char* password_hash) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT EXISTS(SELECT 1 FROM account WHERE username=? AND password_hash=?)";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "account_validate", "Failed to prepare statement to validate account in database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    mlog(ERROR, "account_validate", "Failed to bind username to validate account in database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, password_hash, (int)strlen(password_hash), NULL) != SQLITE_OK) {
    mlog(ERROR, "account_validate", "Failed to bind password hash to validate account in database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_step(res) != SQLITE_ROW) {
    mlog(ERROR, "account_validate", "Failed to retrieve any rows to validate account in database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  int exists = sqlite3_column_int(res, 0);

  sqlite3_finalize(res);

  return exists == 1 ? 0 : -1;
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
