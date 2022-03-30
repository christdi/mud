#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/command.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/data/deallocate.h"
#include "mud/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/lua/script.h"
#include "mud/player.h"

/**
 * Allocate memory for and initialize a command_t.
 *
 * Returns the newly allocated command_t
 **/
command_t* command_new_command_t(const char* uuid, const char* name, const char* script_uuid) {
  command_t* command = calloc(1, sizeof(command_t));

  command->uuid = str_uuid(uuid);
  command->name = strdup(name);
  command->script = str_uuid(script_uuid);

  return command;
}

/**
 * Frees an allocated command_t.
 **/
void command_free_command_t(command_t* command) {
  assert(command);

  if (command->name != NULL) {
    free(command->name);
  }

  free(command);
}

/**
 * Deallocator for data structures.  Data structures only store void pointers so we need
 * to cast to the actual type and pass it to the relevant free function.
 **/
void command_deallocate_command_t(void* value) {
  assert(value);

  command_t* command = (command_t*)value;

  command_free_command_t(command);
}

/**
 * Allocates a new instance of a command_group_t.
 *
 * uuid - The uuid of the command group
 * description - The description of the command group
 *
 * Returns the newly allocated command_group_t
**/
command_group_t* command_new_command_group_t(const char* uuid, const char* description) {
  assert(uuid);
  assert(description);

  command_group_t* command_group = calloc(1, sizeof(command_group_t));

  command_group->uuid = str_uuid(uuid);
  command_group->description = strdup(description);
  command_group->commands = create_linked_list_t();
  command_group->commands->deallocator = deallocate;

  return command_group;
}

/**
 * Frees an allocated command_group_t.
 *
 * group - The command_group_t to free
**/
void command_free_command_group_t(command_group_t* group) {
  assert(group);

  if (group->description != NULL) {
    free(group->description);
  }

  if (group->commands != NULL) {
    free_linked_list_t(group->commands);
  }

  free(group);
}

/**
 * Deallocates a void pointer to a command_group_t.
 *
 * value - The void pointer to the command_group_t to deallocate
**/
void command_deallocate_command_group_t(void* value) {
  assert(value);

  command_free_command_group_t(value);
}

/**
 * Allocates a new instance of a command_repository_t.
 *
 * Returns the newly allocated command_repository_t
**/
command_repository_t* command_new_command_repository_t() {
  command_repository_t* repository = calloc(1, sizeof(command_repository_t));

  repository->dirty = false;
  
  repository->commands = create_linked_list_t();
  repository->commands->deallocator = deallocate;

  repository->groups = create_linked_list_t();
  repository->groups->deallocator = deallocate;

  return repository;
}

/**
 * Frees an allocated command_repository_t.
 *
 * repository - The command_repository_t to free
**/
void command_free_command_repository_t(command_repository_t* repository) {
  assert(repository);

  if (repository->commands != NULL) {
    free_linked_list_t(repository->commands);
  }

  if (repository->groups != NULL) {
    free_linked_list_t(repository->groups);
  }

  free(repository);
}

/**
 * Deallocates a void pointer to a command_repository_t.
 *
 * value - The void pointer to the command_repository_t to deallocate 
**/
void command_deallocate_command_repository_t(void* value) {
  assert(value);

  command_free_command_repository_t(value);
}

/**
 * Retrieves all commands from persistence and places them into game instance
 *
 * game - game_t instance containing database and commands
 *
 * Returns 0 on success or -1 on failure
 **/
int command_load_commands(game_t* game) {
  linked_list_t* results = create_linked_list_t();

  int count;

  if ((count = db_command_load_all(game->database, results)) == -1) {
    LOG(ERROR, "Failed to load commands into engine");
    free_linked_list_t(results);

    return -1;
  }

  lua_call_commands_loaded_hook(game->lua_state, results);

  it_t it = list_begin(results);
  command_t* command = NULL;

  while ((command = it_get(it)) != NULL) {
    hash_table_insert(game->commands, uuid_str(&command->uuid), command);

    it = it_next(it);
  }

  LOG(INFO, "Loaded [%d] commands", count);

  free_linked_list_t(results);

  return 0;
}

/**
 * Retrieves all command groups from persistence and places them into game instance
 *
 * game - game_t instance containing database and commands groups
 *
 * Returns 0 on success or -1 on failure
 **/
int command_load_command_groups(game_t* game) {
  linked_list_t* results = create_linked_list_t();

  int count;

  if ((count = db_command_group_load_all(game->database, results)) == -1) {
    LOG(ERROR, "Failed to load command groups into engine");
    free_linked_list_t(results);

    return -1;
  }

  lua_call_command_groups_loaded_hook(game->lua_state, results);

  it_t it = list_begin(results);
  command_group_t* group = NULL;

  while ((group = it_get(it)) != NULL) {
    hash_table_insert(game->command_groups, uuid_str(&group->uuid), group);

    it = it_next(it);
  }

  LOG(INFO, "Loaded [%d] command groups", count);

  free_linked_list_t(results);

  return 0;
}

/**
 * Adds a command_group_t instance to a command_repository_t instance and marks the repository as dirty.
 *
 * repository - The command_repository_t instance to add the command_group_t to
 * group - The command_group_t instance to add to the repository
 *
 * Returns 0 on success or -1 on failure
**/
int command_add_group_to_repository(command_repository_t* repository, command_group_t* group) {
  assert(repository);
  assert(group);

  if (list_add(repository->groups, (char*)uuid_str(&group->uuid)) == -1) {
    LOG(ERROR, "Failed to add command group to repository");

    return -1;
  }

  repository->dirty = true;

  return 0;
}

/**
 * Removes a command_group_t instance from a command_repository_t instance and marks the repository as dirty.
 *
 * repository - The command_repository_t instance to remove the command_group_t from
 * group - The command_group_t instance to remove from the repository
 *
 * Returns 0 on success or -1 on failure
**/
int command_remove_group_from_repository(command_repository_t* repository, command_group_t* group) {
  assert(repository);
  assert(group);

  list_remove(repository->groups, (char*)uuid_str(&group->uuid));

  repository->dirty = true;

  return 0;
}

/**
 * Updates the commands referenced within a command_repository_t instance to reflect the groups assigned.
 *
 * repository - The command_repository_t instance to update
 * groups - The hash_table_t instance containing all command groups loaded
 * commands - The hash_table_t instance containing all commands loaded
 *
 * Returns 0 on success or -1 on failure
**/
int command_update_commands_in_repository(command_repository_t* repository, hash_table_t* command_groups, hash_table_t* commands) {
  assert(repository);

  if (!repository->dirty) {
    return 0;
  }

  list_clear(repository->commands);

  it_t it = list_begin(repository->groups);
  char* group_uuid = NULL;

  while ((group_uuid = it_get(it)) != NULL) {
    command_group_t* group = hash_table_get(command_groups, group_uuid);

    if (group == NULL) {
      LOG(ERROR, "Failed to find command group [%s]", group_uuid);

      it = it_next(it);

      continue;
    }

    it_t command_it = list_begin(group->commands);
    char* cmd_uuid = NULL;

    while ((cmd_uuid = it_get(command_it)) != NULL) {
      command_t* command = hash_table_get(commands, cmd_uuid);

      if (command == NULL) {
        LOG(ERROR, "Failed to find command [%s]", cmd_uuid);

        command_it = it_next(command_it);

        continue;
      }

      if (list_add(repository->commands, command) == -1) {
        LOG(ERROR, "Failed to add command [%s] to repository", cmd_uuid);

        command_it = it_next(command_it);

        continue;
      }

      command_it = it_next(command_it);
    }

    it = it_next(it);
  }

  repository->dirty = false;

  return 0;
}
