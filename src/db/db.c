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
#include "mud/lua/script.h"
#include "mud/state/state.h"

static int db_account_load_data(sqlite3* db, const char* username, account_t* account);
static int db_account_load_entities(sqlite3* db, const char* username, account_t* account);

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
static int db_account_load_data(sqlite3* db, const char* username, account_t* account) {
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
static int db_account_load_entities(sqlite3* db, const char* username, account_t* account) {
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

  // uuid TEXT PRIMARY KEY,
  // name TEXT NOT NULL,
  // function TEXT NOT NULL,
  // script_uuid TEXT NOT NULL,
  // FOREIGN KEY(script_uuid) REFERENCES script(uuid)

  const char* sql = "SELECT uuid, name, function, script_uuid FROM command WHERE name = ?";

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

    command->uuid = str_uuid((char*)sqlite3_column_text(res, 0));
    command->name = strdup((char*)sqlite3_column_text(res, 1));
    command->function = strdup((char*)sqlite3_column_text(res, 2));
    command->script = str_uuid((char*)sqlite3_column_text(res, 3));

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

    strlcpy(entity->id.raw, (char*)sqlite3_column_text(res, 0), sizeof(entity->id.raw));
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

  if (sqlite3_bind_text(res, 1, entity->id.raw, (int)strlen(entity->id.raw), NULL) != SQLITE_OK) {
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

/**
 * Retrieves an entry from the script table based on the UUID and populates the
 * provided script_t struct.
 * 
 * Parameters
 *   db - sqlite3 handle
 *   uuid - uuid of the script to find
 *   script - script_t to be populated
 * 
 * Returns 0 on success or -1 on faiilure
**/
int db_script_load(sqlite3* db, const char* uuid, script_t* script) {
  assert(db);
  assert(uuid);
  assert(script);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, filepath, allow_std_lib, allow_db_api, allow_game_api, allow_log_api, allow_player_api, allow_script_api, allow_command_api FROM script WHERE uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve script from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, uuid, (int)strlen(uuid), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to retrieve script from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    LOG(ERROR, "Failed to retreive script from database: [%s]", sqlite3_errmsg(db));

    sqlite3_finalize(res);

    return -1;
  }

  script->uuid = str_uuid((char *)sqlite3_column_text(res, 0));
  script->filepath = strdup((char *)sqlite3_column_text(res, 1));

  script_set_permission(script, ALLOW_STD_LIB, sqlite3_column_int(res, 2));
  script_set_permission(script, ALLOW_DB_API, sqlite3_column_int(res, 3));
  script_set_permission(script, ALLOW_GAME_API, sqlite3_column_int(res, 4));
  script_set_permission(script, ALLOW_LOG_API, sqlite3_column_int(res, 5)); // NOLINT(readability-magic-numbers)
  script_set_permission(script, ALLOW_PLAYER_API, sqlite3_column_int(res, 6)); // NOLINT(readability-magic-numbers)
  script_set_permission(script, ALLOW_SCRIPT_API, sqlite3_column_int(res, 7)); // NOLINT(readability-magic-numbers)
  script_set_permission(script, ALLOW_COMMAND_API, sqlite3_column_int(res, 8)); // NOLINT(readability-magic-numbers)

  sqlite3_finalize(res);

  return 0;
}

/**
 * TODO(Chris I)
**/
int db_script_load_all(sqlite3* db, linked_list_t *scripts) {
  assert(db);
  assert(scripts);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, filepath, allow_std_lib, allow_db_api, allow_game_api, allow_log_api, allow_player_api, allow_script_api, allow_command_api FROM script";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve scripts from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive scripts from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return -1;
    }

    script_t* script = create_script_t();

    script->uuid = str_uuid((char *)sqlite3_column_text(res, 0));
    script->filepath = strdup((char *)sqlite3_column_text(res, 1));

    script_set_permission(script, ALLOW_STD_LIB, sqlite3_column_int(res, 2));
    script_set_permission(script, ALLOW_DB_API, sqlite3_column_int(res, 3));
    script_set_permission(script, ALLOW_GAME_API, sqlite3_column_int(res, 4));
    script_set_permission(script, ALLOW_LOG_API, sqlite3_column_int(res, 5)); // NOLINT(readability-magic-numbers)
    script_set_permission(script, ALLOW_PLAYER_API, sqlite3_column_int(res, 6)); // NOLINT(readability-magic-numbers)
    script_set_permission(script, ALLOW_SCRIPT_API, sqlite3_column_int(res, 7)); // NOLINT(readability-magic-numbers)
    script_set_permission(script, ALLOW_COMMAND_API, sqlite3_column_int(res, 8)); // NOLINT(readability-magic-numbers)

    list_add(scripts, script);
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * TODO(Chris I)
**/
int db_state_load(sqlite3* db, const char* uuid, state_t* state) {
  assert(db);
  assert(uuid);
  assert(state);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, name, on_enter, on_exit, on_input, on_tick, script_uuid FROM state WHERE uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve state from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, uuid, (int)strlen(uuid), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to retrieve state from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    LOG(ERROR, "Failed to retreive state from database: [%s]", sqlite3_errmsg(db));

    sqlite3_finalize(res);

    return -1;
  }

  state->uuid = str_uuid((char *)sqlite3_column_text(res, 0));
  state->name = strdup((char *)sqlite3_column_text(res, 1));
  state->on_enter = strdup((char *)sqlite3_column_text(res, 2));
  state->on_exit = strdup((char *)sqlite3_column_text(res, 3));
  state->on_input = strdup((char *)sqlite3_column_text(res, 4));
  state->on_tick = strdup((char *)sqlite3_column_text(res, 5)); // NOLINT(readability-magic-numbers)
  state->script = str_uuid((char *)sqlite3_column_text(res, 6)); // NOLINT(readability-magic-numbers)

  sqlite3_finalize(res);

  return 0;
}

int db_state_load_by_name(sqlite3* db, const char* name, state_t* state) {
  assert(db);
  assert(name);
  assert(state);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, name, on_enter_function, on_exit_function, on_input_function, on_tick_function, script_uuid FROM state WHERE name = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve state from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, name, (int)strlen(name), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind name to retrieve state from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    LOG(ERROR, "Failed to retreive state from database: [%s]", sqlite3_errmsg(db));

    sqlite3_finalize(res);

    return -1;
  }

  state->uuid = str_uuid((char *)sqlite3_column_text(res, 0));
  state->name = strdup((char *)sqlite3_column_text(res, 1));
  state->on_enter = sqlite3_column_type(res, 2) == SQLITE_NULL ? NULL : strdup((char *)sqlite3_column_text(res, 2));
  state->on_exit = sqlite3_column_type(res, 3) == SQLITE_NULL ? NULL : strdup((char *)sqlite3_column_text(res, 3));
  state->on_input = sqlite3_column_type(res, 4) == SQLITE_NULL ? NULL : strdup((char *)sqlite3_column_text(res, 4));
  state->on_tick = sqlite3_column_type(res, 5) == SQLITE_NULL ? NULL : strdup((char *)sqlite3_column_text(res, 5)); // NOLINT(readability-magic-numbers)
  state->script = str_uuid((char *)sqlite3_column_text(res, 6)); // NOLINT(readability-magic-numbers)

  sqlite3_finalize(res);

  return 1;
}
