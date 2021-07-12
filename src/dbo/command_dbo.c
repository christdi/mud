#include <stdlib.h>
#include <assert.h>
#include <sqlite3.h>
#include <string.h>

#include "bsd/string.h"

#include "mud/game.h"
#include "mud/log.h"
#include "mud/dbo/command_dbo.h"
#include "mud/data/linked_list.h"

/**
 * Allocates and returns a pointer to a new command_dbo.
**/
command_dbo_t* create_command_dbo_t() {
  command_dbo_t* command_dbo = calloc(1, sizeof *command_dbo);

  return command_dbo;

}

/**
 * Frees an allocated command_dbo
**/
void free_command_dbo_t(command_dbo_t* command_dbo) {
  assert(command_dbo);

  free(command_dbo);
}

/**
 * Deallocator for command_dbo_t in data structures.
 *
 * Parameters
 *  value - a void pointer to a command_dbo_t structure
**/
void deallocate_command_dbo_t(void* value) {
  assert(value);

  command_dbo_t* command_dbo = (command_dbo_t*)value;

  free_command_dbo_t(command_dbo);
}

/**
 * Searches the database for commands matching a given name.
 *
 * Parameters
 *  game - the game struct containing the database handler
 *  name - the name of the command we're searching for
 *  commands - a pointer to a linked list to be populated with the commands
 *
 * Returns the amount of commands found or -1 on error
**/
int get_commands_by_name(game_t* game, const char* name, linked_list_t* commands) {
  sqlite3_stmt* res = NULL;

  const char* sql = "SELECT name, function FROM command WHERE name = ?";

  if (sqlite3_prepare_v2(game->database, sql, -1, &res, 0) != SQLITE_OK) {
    zlog_error(dc, "Failed to prepare statement to retrieve commands from database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  if (sqlite3_bind_text(res, 1, name, (int)strlen(name), NULL) != SQLITE_OK) {
    zlog_error(dc, "Failed to bind command name to retrieve commands from database: [%s]", sqlite3_errmsg(game->database));
    sqlite3_finalize(res);

    return -1;
  }

  int rc = 0;
  int count = 0;

  while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW) {
      zlog_error(dc, "Failed to retreive commands from database: [%s]", sqlite3_errmsg(game->database));

      sqlite3_finalize(res);

      return 0;
    }

    command_dbo_t* command_dbo = create_command_dbo_t();

    if (strlcpy(command_dbo->name, (char *) sqlite3_column_text(res, 0), COMMAND_NAME_LENGTH) > COMMAND_NAME_LENGTH) {
      zlog_error(nc, "Error retrieving commands from database. Command name length for command [%s] was longer than max length [%d] and was truncated", name, COMMAND_NAME_LENGTH);
    }

    if (strlcpy(command_dbo->function, (char *)sqlite3_column_text(res, 1), COMMAND_FUNCTION_LENGTH) > COMMAND_FUNCTION_LENGTH) {
      zlog_error(nc, "Error retrieving commands from database. Command name length for command [%s] was longer than max length [%d] and was truncated", name, COMMAND_NAME_LENGTH);
    }

    list_add(commands, (void *)command_dbo);

    count++;
  }

  sqlite3_finalize(res);

  return count;
}
