#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/data/linked_list.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/lua/ref.h"
#include "mud/task.h"

int task_is_ready_to_execute(void* value);

/**
 * Allcoates a new task_t
 *
 * Returns the newly allocated task_t
 **/
task_t* task_new_task_t(const char* name, int execute_in, lua_ref_t* ref) {
  task_t* task = calloc(1, sizeof(task_t));

  task->uuid = new_uuid();
  task->name = strdup(name);
  task->execute_at = time(NULL) + execute_in;
  task->ref = ref;

  return task;
}

/**
 * Frees an allocated task_t
 **/
void task_free_task_t(task_t* task) {
  assert(task);

  if (task->name != NULL) {
    free(task->name);
  }

  if (task->ref != NULL) {
    lua_free_lua_ref_t(task->ref);
  }

  free(task);
}

/**
 * Deallocator function for use with a linked list to free allocated task_t types.
 *
 * Parameters
 *  value - a void* that should be safe to cast to task_t*. Behaviour is undefined if not.
 **/
void task_deallocate_task_t(void* value) {
  assert(value);

  task_free_task_t(value);
}

/**
 * Adds a task to the list represented by task_list to execute the supplies in n seconds_in_future.
 *
 * Parameters
 *  tasks - a linked list holding tasks to be executed
 *  task - the task to be scheduled
 *
 * Returns a 0 on success or -1 on failure
 **/
int task_schedule_task(linked_list_t* tasks, task_t* task) {
  assert(tasks);

  if (list_add(tasks, task) != 0) {
    LOG(ERROR, "Unable to add task to linked list");

    return -1;
  }

  return 0;
}

/**
 * Searches the tasks in the supplied linked list for tasks ready to execute and executes them.
 *
 * Parameters
 *  tasks - a linked list containing potential tasks to be executed
 *  game - game struct containing all game related data
 *
 * Returns 0 on success or -1 on error.
 **/
int task_execute_tasks(linked_list_t* tasks, game_t* game) {
  assert(tasks);

  linked_list_t* ready_tasks = create_linked_list_t();
  ready_tasks->deallocator = task_deallocate_task_t;

  if (list_extract(tasks, ready_tasks, task_is_ready_to_execute) != 0) {
    LOG(ERROR, "Unable to extract ready tasks from linked list");

    free_linked_list_t(ready_tasks);

    return -1;
  }

  it_t it = list_begin(ready_tasks);
  task_t* task = NULL;

  while ((task = (task_t*)it_get(it)) != NULL) {
    lua_call_task_execute_hook(game->lua_state, task);

    it = it_next(it);
  }

  free_linked_list_t(ready_tasks);

  return 0;
}

/**
 * Cancels a task that is pending execution
 *
 * tasks - linked list containing pending tasks
 * task - task to be removed
 *
 * Returns 0 on success
**/
int task_cancel_task(linked_list_t* tasks, game_t* game, task_t* task) {
  assert(tasks);
  assert(game);
  assert(task);

  list_remove(tasks, task);

  return 0;
}

/**
 * Predicate for linked list to determine if a given task is ready to execute or not.
 *
 * Parameters
 *  value - a void pointer which should be safe to cast to a task_t*, behaviour is undefined if not.
 *
 * Returns 1 if this task is ready to execute or 0 otherwise.
 **/
int task_is_ready_to_execute(void* value) {
  task_t* task = value;

  if (time(NULL) > task->execute_at) {
    return 1;
  }

  return 0;
}
