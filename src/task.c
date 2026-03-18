#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mud/data/linked_list.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/lua/ref.h"
#include "mud/task.h"

static int task_is_ready_to_execute(void* value);
static void task_advance_execute_at(task_t* task);

/**
 * Allocates a new one-shot Lua task.
 *
 * execute_in_ms - milliseconds from now before the task fires
 *
 * Returns the newly allocated task_t
 **/
task_t* task_new_task_t(const char* name, long execute_in_ms, lua_ref_t* ref) {
  task_t* task = calloc(1, sizeof(task_t));

  task->uuid = new_uuid();
  task->name = strdup(name);
  task->interval_ms = 0;
  task->ref = ref;
  task->callback = NULL;

  gettimeofday(&task->execute_at, NULL);
  task->execute_at.tv_sec += execute_in_ms / 1000;
  task->execute_at.tv_usec += (execute_in_ms % 1000) * 1000;

  if (task->execute_at.tv_usec >= 1000000) {
    task->execute_at.tv_sec++;
    task->execute_at.tv_usec -= 1000000;
  }

  return task;
}

/**
 * Allocates a recurring engine task backed by a C callback rather than Lua.
 *
 * interval_ms - milliseconds between executions
 * callback    - C function to call each time the task fires
 *
 * Returns the newly allocated task_t
 **/
task_t* task_new_engine_task_t(const char* name, long interval_ms, int (*callback)(game_t*)) {
  task_t* task = calloc(1, sizeof(task_t));

  task->uuid = new_uuid();
  task->name = strdup(name);
  task->interval_ms = interval_ms;
  task->ref = NULL;
  task->callback = callback;

  gettimeofday(&task->execute_at, NULL);
  task->execute_at.tv_sec += interval_ms / 1000;
  task->execute_at.tv_usec += (interval_ms % 1000) * 1000;

  if (task->execute_at.tv_usec >= 1000000) {
    task->execute_at.tv_sec++;
    task->execute_at.tv_usec -= 1000000;
  }

  return task;
}

/**
 * Frees an allocated task_t
 **/
void task_free_task_t(task_t* task) {
  assert(task);

  free(task->name);

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
 * Adds a task to the list represented by task_list.
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
 * Recurring tasks (interval_ms > 0) are rescheduled after execution; one-shot tasks are freed.
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

  linked_list_t* to_reschedule = create_linked_list_t();
  to_reschedule->deallocator = NULL;

  it_t it = list_begin(ready_tasks);
  task_t* task = NULL;

  while ((task = (task_t*)it_get(it)) != NULL) {
    if (task->callback != NULL) {
      task->callback(game);
    } else {
      lua_call_task_execute_hook(game->lua_state, task);
    }

    if (task->interval_ms > 0) {
      list_add(to_reschedule, task);
    }

    it = it_next(it);
  }

  it = list_begin(to_reschedule);
  while ((task = (task_t*)it_get(it)) != NULL) {
    list_steal(ready_tasks, task);
    task_advance_execute_at(task);
    task_schedule_task(tasks, task);

    it = it_next(it);
  }

  free_linked_list_t(to_reschedule);
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
static int task_is_ready_to_execute(void* value) {
  task_t* task = value;

  struct timeval now;
  gettimeofday(&now, NULL);

  if (now.tv_sec > task->execute_at.tv_sec) {
    return 1;
  }

  if (now.tv_sec == task->execute_at.tv_sec && now.tv_usec >= task->execute_at.tv_usec) {
    return 1;
  }

  return 0;
}

/**
 * Advances execute_at by interval_ms for a recurring task.
 **/
static void task_advance_execute_at(task_t* task) {
  task->execute_at.tv_sec += task->interval_ms / 1000;
  task->execute_at.tv_usec += (task->interval_ms % 1000) * 1000;

  if (task->execute_at.tv_usec >= 1000000) {
    task->execute_at.tv_sec++;
    task->execute_at.tv_usec -= 1000000;
  }
}
