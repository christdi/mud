#include <assert.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "bsd/string.h"

#include "mud/command/command.h"
#include "mud/data/linked_list.h"
#include "mud/db/db.h"
#include "mud/ecs/entity.h"
#include "mud/account.h"
#include "mud/log.h"

int db_account_load_data(sqlite3* db, const char* username, account_t* account);
int db_account_load_entities(sqlite3* db, const char* username, account_t* account);

/**
 * Persists an account to the database.  If the account already exists it will be updated.
 *
 * Parameters
 *   db - Handle to sqlite database
 *   account - The account to be persisted
 *
 * This function returns 0 on success or -1 on failure.
**/
int db_account_save(sqlite3* db, account_t* account) {
  sqlite3_stmt* res = NULL;

  const char* sql = "INSERT INTO account(username, password_hash) VALUES(?, ?) "
                    "ON CONFLICT(username) DO UPDATE SET username = excluded.username, password_hash = excluded.password_hash";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, account->username, (int)strlen(account->username), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind username to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, account->password_hash, (int)strlen(account->password_hash), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind password hash to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  };

  if (sqlite3_step(res) != SQLITE_DONE) {
    LOG(ERROR, "Failed to insert account into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * Loads the base data of an account from persistence.
 *
 * Parameters
 *   db - Handle to sqlite database
 *   username - Username of the account to load
 *   account - Account to be populated with loaded data
 *
 * Returns 0 on success or -1 on failure
**/
int db_account_load_data(sqlite3* db, const char* username, account_t* account) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT username, password_hash FROM account WHERE username=?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to load account from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind username to retrieve account from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    LOG(ERROR, "Failed to retreive account from database: [%s]", sqlite3_errmsg(db));

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
 * Loads the entities assigned to an account.
 *
 * Parameters:
 *   db - Handle to the sqlite database
 *   username - Username of the account to load
 *   account - Account to be populated with data
 *
 * Returns 0 on success or -1 on failure.
**/
int db_account_load_entities(sqlite3* db, const char* username, account_t* account) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT entity_uuid FROM account_entity WHERE account_username=?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to load account entities from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind username to retrieve account entities from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive account entities from database: [%s]", sqlite3_errmsg(db));

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
 * Loads an account from persistence.
 *
 * Parameters:
 *   db - Handle to sqlite database
 *   username - Username of the account to be loaded
 *   account - Account to be populated with data
 *
 * Returns 0 on success or -1 on failure.
**/
int db_account_load(sqlite3* db, const char* username, account_t* account) {
  if (db_account_load_data(db, username, account) != 0) {
    LOG(ERROR, "Failed to load account data");
    return -1;
  }

  if (db_account_load_entities(db, username, account) < 0) {
    LOG(ERROR, "Failed to load account entities");
    return -1;
  }

  return 0;
}

/**
 * Determines if an account exists in persistence.
 *
 * Parameters:
 *   db - Handle to sqlite database
 *   username - Username to find account whose existence is to be confirmed
 *
 * Returns 0 if account exists or -1 otherwise
**/
int db_account_exists(sqlite3* db, const char* username) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT EXISTS(SELECT 1 FROM account WHERE username=?)";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to validate account in database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind username to validate account in database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_step(res) != SQLITE_ROW) {
    LOG(ERROR, "Failed to retrieve any rows to validate account in database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int exists = sqlite3_column_int(res, 0);

  sqlite3_finalize(res);

  return exists == 1 ? 0 : -1;
}

/**
 * Retrieves a command via it's name
 *
 * Parameters:
 *   db - Handle to sqlite database
 *   name - Name of the command to be retrieced
 *   results - Linked list to be populated with matching commands
 *
 * Returns number of results on success or -1 on failure.
**/
int db_command_find_by_name(sqlite3* db, const char* name, linked_list_t* results) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT name, function FROM command WHERE name = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve commands from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, name, (int)strlen(name), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind command name to retrieve commands from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive commands from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return 0;
    }

    command_t* command = create_command_t();

    command->name = strdup((char*)sqlite3_column_text(res, 0));
    command->function = strdup((char*)sqlite3_column_text(res, 1));

    list_add(results, (void*)command);

    count++;
  }

  sqlite3_finalize(res);

  return count;  
}

/**
 * Loads all entiites from persistence.
 *
 * Parameters
 *   db - Handle to sqlite database
 *   entities - Linked list to be populated with entities
 *
 * Returns number of results on success or -1 on failure.
**/
int db_entity_load_all(sqlite3* db, linked_list_t *entities) {
  assert(db);
  assert(entities);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, name, description FROM entity";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve entities from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive entities from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return 0;
    }

    entity_t* entity = create_entity_t();

    strlcpy(entity->id.uuid, (char*)sqlite3_column_text(res, 0), UUID_SIZE);
    entity->name = strdup((char*)sqlite3_column_text(res, 1));
    entity->description = strdup((char*)sqlite3_column_text(res, 2));

    list_add(entities, (void*)entity);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * Persists an entity to the database.  If the entity exists already it will be updated.
 *
 * Parameters
 *   db - Handle to sqlite database
 *   entity - Entity to be saved
 *
 * Returns 0 on success or -1 on failure.
**/
int db_entity_save(sqlite3* db, entity_t* entity) {
  assert(db);
  assert(entity);

  sqlite3_stmt* res = NULL;

  const char* sql = "INSERT INTO entity(uuid, name, description) VALUES(?, ?, ?) "
                    "ON CONFLICT(uuid) DO UPDATE SET name = excluded.name, description = excluded.description";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to insert entity into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, entity->id.uuid, (int)strlen(entity->id.uuid), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to insert entity into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, entity->name, (int)strlen(entity->name), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind name to insert entity into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  };

  if (sqlite3_bind_text(res, 3, entity->description, (int)strlen(entity->description), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind description to insert entity into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  };

  if (sqlite3_step(res) != SQLITE_DONE) {
    LOG(ERROR, "Failed to insert entity into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}
