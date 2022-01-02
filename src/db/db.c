#include <assert.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "mud/data/linked_list.h"
#include "mud/db/db.h"
#include "mud/account.h"
#include "mud/log.h"

int db_account_load_data(sqlite3* db, const char* username, account_t* account);
int db_account_load_entities(sqlite3* db, const char* username, account_t* account);

/**
 * TODO: Populate
**/
int db_account_save(sqlite3* db, account_t* account) {
  sqlite3_stmt* res = NULL;

  const char* sql = "INSERT INTO account(username, password_hash) VALUES(?, ?) "
                    "ON CONFLICT(username) DO UPDATE SET username = excluded.username, password_hash = excluded.password_hash";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "db_account_save", "Failed to prepare statement to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, account->username, (int)strlen(account->username), NULL) != SQLITE_OK) {
    mlog(ERROR, "db_account_save", "Failed to bind username to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, account->password_hash, (int)strlen(account->password_hash), NULL) != SQLITE_OK) {
    mlog(ERROR, "db_account_save", "Failed to bind password hash to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  };

  if (sqlite3_step(res) != SQLITE_DONE) {
    mlog(ERROR, "db_account_save", "Failed to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * TODO: Populate
**/
int db_account_load_data(sqlite3* db, const char* username, account_t* account) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT username, password_hash FROM account WHERE username=?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "db_account_load_data", "Failed to prepare statement to load account from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    mlog(ERROR, "db_account_load_data", "Failed to bind username to retrieve account from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    mlog(ERROR, "db_account_load_data", "Failed to retreive account from database: [%s]", sqlite3_errmsg(db));

    sqlite3_finalize(res);

    return -1;
  }

  if (account->username != NULL) {
    free(account->username);
  }

  if (account->password_hash != NULL) {
    free(account->password_hash);
  }

  account->username = strdup((char*)sqlite3_column_text(res, 0));
  account->password_hash = strdup((char*)sqlite3_column_text(res, 1));

  sqlite3_finalize(res);

  return 0;
}

/**
 * TODO: Populate}
**/
int db_account_load_entities(sqlite3* db, const char* username, account_t* account) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT entity_uuid FROM account_entity WHERE account_username=?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "db_account_load_entities", "Failed to prepare statement to load account entities from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    mlog(ERROR, "db_account_load_entities", "Failed to bind username to retrieve account entities from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      mlog(ERROR, "db_account_load_entities", "Failed to retreive account entities from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return 0;
    }

    list_add(account->entities, strdup((char*)sqlite3_column_text(res, 0)));

    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * TODO: Populate
**/
int db_account_load(sqlite3* db, const char* username, account_t* account) {
  if (db_account_load_data(db, username, account) != 0) {
    mlog(ERROR, "db_account_load", "Failed to load account data");
    return -1;
  }

  if (db_account_load_entities(db, username, account) < 0) {
    mlog(ERROR, "db_account_load", "Failed to load account entities");
    return -1;
  }

  return 0;
}

/**
 * TODO: Populate
**/
int db_account_exists(sqlite3* db, const char* username) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT EXISTS(SELECT 1 FROM account WHERE username=?)";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "db_account_exists", "Failed to prepare statement to validate account in database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    mlog(ERROR, "db_account_exists", "Failed to bind username to validate account in database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_step(res) != SQLITE_ROW) {
    mlog(ERROR, "db_account_exists", "Failed to retrieve any rows to validate account in database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int exists = sqlite3_column_int(res, 0);

  sqlite3_finalize(res);

  return exists == 1 ? 0 : -1;
}
