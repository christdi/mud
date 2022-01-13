#include <assert.h>
#include <stdlib.h>

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/log.h"
#include "mud/lua/script.h"
#include "mud/lua/repository.h"
#include "mud/util/muduuid.h"

static void process_pending_removal(script_repository_t* repository);
static void process_pending_addition(script_repository_t* repository);

/**
 * TODO(Chris I)
**/
script_repository_t* create_script_repository_t() {
  script_repository_t* repository = calloc(1, sizeof *repository);

  repository->scripts = create_hash_table_t();
  repository->scripts->deallocator = deallocate_script;

  repository->pending_add = create_linked_list_t();
  repository->pending_add->deallocator = deallocate_script;

  repository->pending_rem = create_linked_list_t();
  repository->pending_rem->deallocator = deallocate_script;

  return repository;
}

/**
 * TODO(Chris I)
**/
void free_script_repository_t(script_repository_t* repository) {
  assert(repository);

  if (repository->scripts) {
    free_hash_table_t(repository->scripts);
  }

  if (repository->pending_add) {
    free_linked_list_t(repository->pending_add);
  }

  if (repository->pending_rem) {
    free_linked_list_t(repository->pending_rem);
  }

  free(repository);
}

/**
 * TODO(Chris I)
**/
int script_repository_delete(script_repository_t* repository, const char* uuid) {
  assert(repository);
  assert(uuid);

  script_t* script = NULL;

  if ((script = hash_table_get(repository->scripts, uuid)) == NULL) {
    LOG(ERROR, "Error retrieving script with uuid [%s] for deletion", uuid);

    return -1;
  }

  if (list_add(repository->pending_rem, script) == -1) {
    LOG(ERROR, "Error inserting script with uuid [%s] into script repository pending removal", uuid);

    return -1;
  }

  return 0;
}

/**
 * TODO(Chris I)
**/
int script_repository_load(script_repository_t* repository, game_t* game, const char* uuid, script_t** script_out) {
  assert(repository);
  assert(uuid);

  if (hash_table_has(repository->scripts, uuid) == 1) {
    script_t* script = hash_table_get(repository->scripts, uuid);

    if (script_out != NULL) {
      *script_out = script;
    }

    return 0;
  }

  if (script_load(game, uuid, script_out) == -1) {
    LOG(ERROR, "Failed to load script with uuid [%s]", uuid);

    return -1;
  }

  if (list_add(repository->pending_add, *script_out) == -1) {
    LOG(ERROR, "Failed to add script [%s] to pending addition in script repository", uuid);

    return -1;
  }

  return 0;
}

/**
 * TODO(Chris I)
**/
int script_repository_reload(script_repository_t* repository, game_t* game, const char* uuid, script_t** script_out) {
  assert(repository);
  assert(uuid);

  if (script_repository_delete(repository, uuid) == -1) {
    LOG(ERROR, "Failed to delete script with uuid [%s] for reload", uuid);

    return -1;
  }

  if (script_load(game, uuid, script_out) == -1) {
    LOG(ERROR, "Failed to load script with uuid [%s]", uuid);

    return -1;
  }

  if (list_add(repository->pending_add, *script_out) == -1) {
    LOG(ERROR, "Failed to add script [%s] to pending addition in script repository", uuid);

    return -1;
  }

  return 0;
}

/**
 * TODO(Chris I)
**/
void script_repository_update(script_repository_t* repository) {
  assert(repository);

  process_pending_removal(repository);
  process_pending_addition(repository);
}

/**
 * TODO(Chris I)
**/
static void process_pending_removal(script_repository_t* repository) {
  assert(repository);

  if (list_size(repository->pending_rem) == 0) {
    return;
  }

  script_t* script = NULL;

  it_t it = list_begin(repository->pending_rem);

  while((script = it_get(it)) != NULL) {
    const char* uuid = uuid_str(&script->uuid);

    if (hash_table_has(repository->scripts, uuid)) {
      hash_table_delete(repository->scripts, uuid);

      it = list_steal(repository->pending_rem, script);

      continue;
    }

    it = list_remove(repository->pending_rem, script);
  }
}

/**
 * TODO(Chris I)
**/
static void process_pending_addition(script_repository_t* repository) {
  assert(repository);

  if (list_size(repository->pending_add) == 0) {
    return;
  }

  script_t* script = NULL;

  it_t it = list_begin(repository->pending_add);

  while((script = it_get(it)) != NULL) {
    const char* uuid = uuid_str(&script->uuid);

    if (hash_table_has(repository->scripts, uuid)) {
      hash_table_delete(repository->scripts, uuid);
    }

    if (hash_table_insert(repository->scripts, uuid, script) != 0) {
      LOG(ERROR, "Error inserting script with uuid [%s] into script repository", uuid);

      it = list_remove(repository->pending_add, script);

      continue;
    }

    it = list_steal(repository->pending_add, script);
  }
}
