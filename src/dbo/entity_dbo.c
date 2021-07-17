#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "mud/data/linked_list.h"
#include "mud/dbo/entity_dbo.h"
#include "mud/ecs/entity.h"
#include "mud/game.h"
#include "mud/log.h"

/**
 * Allocates a new entity_dbo_t instance.
 *
 * Returns the newly allocated instance.
**/
entity_dbo_t* entity_dbo_t_new() {
  entity_dbo_t* entity_dbo = calloc(1, sizeof *entity_dbo);

  return entity_dbo;
}

/**
 * Frees an allocated entity_dbo_t instance.
 *
 * Parameters
 *  entity_dbo - the entity_dbo_t to be freed
**/
void entity_dbo_t_free(entity_dbo_t* entity_dbo) {
  assert(entity_dbo);

  if (entity_dbo->uuid != NULL) {
    free(entity_dbo->uuid);
  }

  if (entity_dbo->name != NULL) {
    free(entity_dbo->name);
  }

  if (entity_dbo->description != NULL) {
    free(entity_dbo->description);
  }

  free(entity_dbo);
}

/**
 * Deallocator for usage with entity_dbo_t in a data structure.
 *
 * Parameters
 *  value - a void* to an entity_dbo_t*, behaviour undefined if it's not
**/
void entity_dbo_t_deallocate(void* value) {
  assert(value);

  entity_dbo_t_free((entity_dbo_t*) value);
}

/**
 * Insert or update an entity_dbo_t to the database.
 *
 * Parameters
 *  game - game containing access to the database
 *  entity_dbo - the entity_dbo_t to be saved to the database
 *
 * Returns -1 on failure or 0 on success.
**/
int entity_dbo_save(game_t* game, entity_dbo_t* entity_dbo) {
  assert(game);
  assert(entity_dbo);

  sqlite3_stmt* res = NULL;

  const char* sql = "INSERT INTO entity(uuid, name, description) VALUES(?, ?, ?) "
                    "ON CONFLICT(uuid) DO UPDATE SET name = excluded.name, description = excluded.description";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "entity_dbo_save", "Failed to prepare statement to insert entity into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, entity_dbo->uuid, (int)strlen(entity_dbo->uuid), NULL) != SQLITE_OK) {
    mlog(ERROR, "entity_dbo_save", "Failed to bind uuid to insert entity into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 2, entity_dbo->name, (int)strlen(entity_dbo->name), NULL) != SQLITE_OK) {
    mlog(ERROR, "entity_dbo_save", "Failed to bind name to insert entity into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  };

  if (sqlite3_bind_text(res, 3, entity_dbo->description, (int)strlen(entity_dbo->description), NULL) != SQLITE_OK) {
    mlog(ERROR, "entity_dbo_save", "Failed to bind description to insert entity into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  };

  if (sqlite3_step(res) != SQLITE_DONE) {
    mlog(ERROR, "entity_dbo_save", "Failed to insert entity into database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  sqlite3_finalize(res);

  return 0;
}

/**
 * Retrieves all entities from the database.
 *
 * Parameters
 *  game - game_t containing sqlite3 database object.
 *  entities - an allocated linked list to add entities to
 *
 * Returns -1 on failure or the amount of entities retrieved
**/
int entity_dbo_load_all(game_t* game, linked_list_t* entities) {
  assert(game);
  assert(entities);

  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT uuid, name, description FROM entity";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    mlog(ERROR, "entity_dbo_load_all", "Failed to prepare statement to retrieve entities from database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      mlog(ERROR, "entity_dbo_load_all", "Failed to retreive entities from database: [%s]", sqlite3_errmsg(game->database));

      sqlite3_finalize(res);

      return 0;
    }

    entity_dbo_t* entity_dbo = entity_dbo_t_new();

    entity_dbo->uuid = strdup((char*)sqlite3_column_text(res, 0));
    entity_dbo->name = strdup((char*)sqlite3_column_text(res, 1));
    entity_dbo->description = strdup((char*)sqlite3_column_text(res, 2));

    list_add(entities, (void*)entity_dbo);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}

/**
 * Populates an instance of entity_dbo_t with data from an entity_t
 *
 * Parameters
 *  entity_dbo - the entity_dbo_t to be populated
 *  entity - the entity_t to be populated from
**/
void entity_dbo_from_entity(entity_dbo_t* entity_dbo, entity_t* entity) {
  assert(entity_dbo);
  assert(entity);

  if (entity->id.uuid[0] != '\0') {
    entity_dbo->uuid = strdup(entity->id.uuid);
  }

  if (entity->name != NULL) {
    entity_dbo->name = strdup(entity->name);
  }

  if (entity->description != NULL) {
    entity_dbo->description = strdup(entity->description);
  }
}
