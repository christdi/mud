#include <assert.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

#include "bsd/string.h"

#include "mud/command/command.h"
#include "mud/data/linked_list.h"
#include "mud/db/db.h"
#include "mud/ecs/entity.h"
#include "mud/log.h"
#include "mud/lua/script.h"
#include "mud/player.h"

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

  const char* sql = "SELECT uuid, name, script_uuid FROM command WHERE name = ?";

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
    command->script = str_uuid((char*)sqlite3_column_text(res, 2));

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
int db_entity_load_all(sqlite3* db, linked_list_t* entities) {
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

    entity_t* entity = ecs_create_entity_t();

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
 * TODO(Chris I)
**/
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
 * Retrieves permissions of a script.
 *
 * Parameters
 *   db - Pointer to sqlite3 database
 *   uuid - UUID of the script access
 *   results - Out parameter for results
 *
 * Returns amount of results or -1 on failure
**/
int db_script_sandbox_permission_by_script_id(sqlite3* db, const char* uuid, linked_list_t* results) {
  assert(db);
  assert(uuid);
  assert(results);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT module, method FROM script_sandbox_permission p INNER JOIN script_sandbox_group_permission gp on gp.permission_uuid = p.uuid INNER JOIN script_group g on g.group_uuid = gp.group_uuid WHERE g.script_uuid = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    LOG(ERROR, "Failed to prepare statement to retrieve script permissions from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, uuid, (int)strlen(uuid), NULL) != SQLITE_OK) {
    LOG(ERROR, "Failed to bind uuid to retrieve script permissions from database: [%s]", sqlite3_errmsg(db));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      LOG(ERROR, "Failed to retreive script permissions from from database: [%s]", sqlite3_errmsg(db));

      sqlite3_finalize(res);

      return -1;
    }

    char* module = sqlite3_column_type(res, 0) == SQLITE_NULL ? NULL : (char*) sqlite3_column_text(res, 0);
    char* method = sqlite3_column_type(res, 1) == SQLITE_NULL ? NULL : (char*) sqlite3_column_text(res, 1);


    script_permission_t* script_permission = script_new_script_permission_t(module, method);

    list_add(results, script_permission);
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
int db_script_script_group_by_script_id(sqlite3* db, const char* uuid, linked_list_t* results) {
  assert(db);
  assert(uuid);
  assert(results);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT name FROM script_sandbox_group ssg INNER JOIN script_group sg on ssg.uuid = sg.group_uuid WHERE sg.script_uuid = ?";

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

    char* name = (char*) sqlite3_column_text(res, 0);

    script_group_t* script_group = script_new_script_group_t(name);

    list_add(results, script_group);
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
