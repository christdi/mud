#include <assert.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "bsd/string.h"

#include "mud/action.h"
#include "mud/command.h"
#include "mud/data/linked_list.h"
#include "mud/db.h"
#include "mud/ecs/entity.h"
#include "mud/log.h"
#include "mud/lua/script.h"
#include "mud/player.h"

static int populate_command_group_commands(sqlite3* db, command_group_t* command_group);

/**
 * Begins a trasnaction.
 *
 * db - sqlite3 handle to database
 *
 * Returns 0 on success or -1 on failure.
**/
int db_begin_transaction(sqlite3* db) {
  if (sqlite3_exec(db, "BEGIN", 0, 0, 0) != 0) {
    LOG(ERROR, "Failed to begin database transaction: [%s]", sqlite3_errmsg(db));

    return -1;
  }

  return 0;
}

/**
 * Ends a transaction, committing the results.
 *
 * db - sqlite3 handle to database
 *
 * Returns 0 on success or -1 on failure
**/
int db_end_transaction(sqlite3* db) {
  if (sqlite3_exec(db, "COMMIT", 0, 0, 0) != 0) {
    LOG(ERROR, "Failed to begin database transaction: [%s]", sqlite3_errmsg(db));

    return -1;
  }

  return 0;
}

/**
 * Retrieves all commands from the database.
 *
 * db - sqlite3 handle to database
 * results - linked list to store results in
 *
 * Returns number of results on success or -1 on failure.
 **/
int db_command_load_all(sqlite3* db, linked_list_t* results) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, name, script_uuid FROM command";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve commands from database: [%s]", sqlite3_errmsg(db));
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

    char* uuid = (char*)sqlite3_column_text(res, 0);
    char* name = (char*)sqlite3_column_text(res, 1);
    char* script_uuid = (char*)sqlite3_column_text(res, 2);

    command_t* command = command_new_command_t(uuid, name, script_uuid);

    list_add(results, command);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * Retrives all command groups from the database.
 * 
 * db - sqlite3 handle to database
 * results - linked list to store results in
 *
 * Returns number of results on success or -1 on failure.
**/
int db_command_group_load_all(sqlite3* db, linked_list_t* results) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, description FROM command_group";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve command groups from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive command groups from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return -1;
    }

    char* uuid = (char*)sqlite3_column_text(res, 0);
    char* name = (char*)sqlite3_column_text(res, 1);

    command_group_t* group = command_new_command_group_t(uuid, name);

    if (populate_command_group_commands(db, group) == -1) {
      LOG(ERROR, "Failed to populate command group commands: [%s]", sqlite3_errmsg(db));
      command_free_command_group_t(group);
      sqlite3_finalize(res);

      return -1;
    }

    list_add(results, group);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * Populates the commands of a command_group_t.
 *
 * db - sqlite3 handle to database
 * command_group - command_group_t to populate
 *
 * Returns 0 on success or -1 on failure.
**/
int populate_command_group_commands(sqlite3* db, command_group_t* command_group) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT command_uuid FROM command_group_command WHERE command_group_uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve command group commands from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, command_group->uuid.raw, -1, SQLITE_STATIC) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind command group uuid to statement: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive command group commands from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return -1;
    }

    char* command_uuid = strdup((char*)sqlite3_column_text(res, 0));
    list_add(command_group->commands, command_uuid);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * Retrieves all actions from database.
 *
 * db - sqlite database handle
 * results - linked list to place query results
 *
 * Returns number of results on success or -1 on failure
 **/
int db_action_load_all(sqlite3* db, linked_list_t* results) {
  assert(db);
  assert(results);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, name, script_uuid FROM action";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve actions from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive actions from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return 0;
    }

    char* uuid = (char*)sqlite3_column_text(res, 0);
    char* name = (char*)sqlite3_column_text(res, 1);
    char* script_uuid = (char*)sqlite3_column_text(res, 2);

    action_t* action = action_new_action_t(uuid, name, script_uuid);
    list_add(results, action);

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
int db_entity_load_all(sqlite3* db, linked_list_t* entities) {
  assert(db);
  assert(entities);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid FROM entity";

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

    entity_t* entity = ecs_new_entity_t();

    strlcpy(entity->id.raw, (char*)sqlite3_column_text(res, 0), sizeof(entity->id.raw));

    list_add(entities, (void*)entity);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * Retrieves the entity ids associated with a given user.
 *
 * db - sqlite database instance
 * uuid - uuid of the user to retrieve entity ids for
 * results - out parameters to place results
 **/
int db_entity_get_ids_by_user(sqlite3* db, const char* uuid, linked_list_t* results) {
  assert(db);
  assert(uuid);
  assert(results);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT entity_uuid FROM user_entity WHERE user_uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve entity ids from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, uuid, (int)strlen(uuid), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to retrieve entity ids from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive entity ids from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return 0;
    }

    list_add(results, strdup((char*)sqlite3_column_text(res, 0)));
    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * Deletes any entries in user_entity matching a given entity UUID.
 *
 * sqlite - sqlite3 handle to database
 * entity - the entity to be deleted to identify rows to delete
 *
 * Returns 0 on success or -1 on failure
**/
int db_entity_delete_user_entity(sqlite3* db, entity_t* entity) {
  assert(db);
  assert(entity);

  sqlite3_stmt* res = NULL;

  const char* sql = "DELETE FROM user_entity WHERE entity_uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to delete user entity from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, entity->id.raw, (int)strlen(entity->id.raw), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to delete user entity from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_step(res) != SQLITE_DONE) {
    LOG(ERROR, "Failed to delete user entity from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * Persists an entity to the database.
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

  const char* sql = "INSERT INTO entity(uuid) VALUES(?)";

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

  if (sqlite3_step(res) != SQLITE_DONE) {
    LOG(ERROR, "Failed to insert entity into database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * Deletes an entity from the database.
 *   db - Handle to sqlite database
 *   entity - Entity to be saved
 *
 * Returns 0 on success or -1 on failure.
**/
int db_entity_delete(sqlite3* db, entity_t* entity) {
  assert(db);
  assert(entity);

  sqlite3_stmt* res = NULL;

  const char* sql = "DELETE FROM entity WHERE uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to delete entity from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, entity->id.raw, (int)strlen(entity->id.raw), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to delete entity from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_step(res) != SQLITE_DONE) {
    LOG(ERROR, "Failed to delete entity from database: [%s]", sqlite3_errmsg(db));
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

  const char* sql = "SELECT uuid, filepath FROM script WHERE uuid = ?";

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

  script->uuid = str_uuid((char*)sqlite3_column_text(res, 0));
  script->filepath = strdup((char*)sqlite3_column_text(res, 1));

  sqlite3_finalize(res);

  return 0;
}

/**
 * Loads all scripts from the database.
 * 
 * db - sqlite3 handle
 * scripts - result set of scripts
 * 
 * Returns 0 on success or -1 on failure
 */
int db_script_load_all(sqlite3* db, linked_list_t* scripts) {
  assert(db);
  assert(scripts);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, filepath FROM script";

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

    script->uuid = str_uuid((char*)sqlite3_column_text(res, 0));
    script->filepath = strdup((char*)sqlite3_column_text(res, 1));

    list_add(scripts, script);
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * Retrieves the script groups associated with a script.
 *
 * db - sqlite3 db instance
 * uuid - uuid of script to retrieve groups for
 * results - out parameter for results
 *
 * Returns number of results or -1 on error.
 **/
int db_script_sandbox_group_by_script_id(sqlite3* db, const char* uuid, linked_list_t* results) {
  assert(db);
  assert(uuid);
  assert(results);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, filepath, name, description FROM script_sandbox_group ssg INNER JOIN script_group sg on ssg.uuid = sg.group_uuid WHERE sg.script_uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve script group from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, uuid, (int)strlen(uuid), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to retrieve script group from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive script group from from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return -1;
    }

    
    char* uuid = (char*)sqlite3_column_text(res, 0);
    char* filepath = (char*)sqlite3_column_text(res, 1);
    char* name = (char*)sqlite3_column_text(res, 2);
    char* description = (char*)sqlite3_column_text(res, 3);

    script_group_t* script_group = script_new_script_group_t(uuid, filepath, name, description);

    list_add(results, script_group);
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * Saves a script_group_t to the database.
 * 
 * db - sqlite3 db instance
 * group - script_group_t to save
 * 
 * Returns 0 on success or -1 on failure.
 */
int db_script_sandbox_group_save(sqlite3* db, script_group_t* group) {
  assert(db);
  assert(group);

  sqlite3_stmt* res = NULL;

  const char *sql = "INSERT INTO script_sandbox_group (uuid, filepath, name, description) VALUES (?, ?, ?, ?)";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to save script group to database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, group->uuid.raw, (int)strlen(group->uuid.raw), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to save script group to database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, group->filepath, (int)strlen(group->filepath), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind filepath to save script group to database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 3, group->name, (int)strlen(group->name), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind name to save script group to database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 4, group->description, (int)strlen(group->description), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind description to save script group to database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_step(res) != SQLITE_DONE) {
    LOG(ERROR, "Failed to save script group to database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * Checks for the existence of a user matching a given username and password_hash.
 *
 * Parameters
 *   db - pointer to sqlite3 database
 *   username - username to query for
 *   password_hash - password_hash to query for
 *
 * Returns 1 if we have a match, 0 if we do not or -1 on error
 **/
int db_user_authenticate(sqlite3* db, const char* username, const char* password_hash) {
  assert(db);
  assert(username);
  assert(password_hash);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT EXISTS(SELECT 1 FROM user WHERE username=? AND password_hash=?)";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to authenticate user from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind name to authenticate user from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, password_hash, (int)strlen(password_hash), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind password hash to authenticate user from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    LOG(ERROR, "Failed to authenticate user from database: [%s]", sqlite3_errmsg(db));

    sqlite3_finalize(res);

    return -1;
  }

  int authenticated = sqlite3_column_int(res, 0);

  sqlite3_finalize(res);

  return authenticated;
}

/**
 * Retrieve a user from the database via their username.
 *
 * Parameters
 *   db - pointer to Sqlite3 instance
 *   username - username to be used for lookup
 *   player - the player to be populated
 **/
int db_user_load_by_username(sqlite3* db, const char* username, player_t* player) {
  assert(db);
  assert(username);
  assert(player);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, username FROM user WHERE username = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve state from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, username, (int)strlen(username), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind name to retrieve state from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  if ((rc = sqlite3_step(res)) != SQLITE_ROW) {
    if (rc == SQLITE_DONE) {
      return 0;
    }

    LOG(ERROR, "Failed to retreive user from database: [%s]", sqlite3_errmsg(db));

    sqlite3_finalize(res);

    return -1;
  }

  player->user_uuid = str_uuid((char*)sqlite3_column_text(res, 0));
  player->username = strdup((char*)sqlite3_column_text(res, 1));

  sqlite3_finalize(res);

  return 1;
}
