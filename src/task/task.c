#include <assert.h>
#include <stdlib.h>

#include "mud/data/linked_list.h"
#include "mud/log.h"
#include "mud/task/task.h"

int task_is_ready_to_execute(void* value);

/**
 * Allcoates a new task_t
 * 
 * Returns the newly allocated task_t
**/
task_t* create_task_t() {
  task_t* task = calloc(1, sizeof *task);

  task->execute_at = 0;
  task->function = NULL;

  return task;
}

/**
 * Frees an allocated task_t
**/
void free_task_t(task_t* task) {
  assert(task);

  free(task);
}

/**
 * Deallocator function for use with a linked list to free allocated task_t types.
 * 
 * Parameters
 *  value - a void* that should be safe to cast to task_t*. Behaviour is undefined if not.
**/
void deallocate_task_t(void* value) {
  if (value != NULL) {
    free_task_t((task_t*)value);
  }
}

/**
 * Adds a task to the list represented by task_list to execute the supplies in n seconds_in_future.
 * 
 * Parameters
 *  tasks - a linked list holding tasks to be executed
 *  seconds_in_future - how many seconds in the future the tasks should be executed
 *  function - the function to be executed when the task is due
 * 
 * Returns a 0 on success or -1 on failure 
**/
int task_schedule(linked_list_t* tasks, int seconds_in_future, task_func_t function) {
  assert(tasks);
  assert(seconds_in_future > 0);
  assert(function);

  task_t* task = create_task_t();

  task->execute_at = time(NULL) + seconds_in_future;
  task->function = function;

  if (list_add(tasks, (void*)task) != 0) {
    zlog_error(gc, "task_schedule(): Unable to add task to linked list");

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
int task_execute(linked_list_t* tasks, game_t* game) {
  assert(tasks);

  linked_list_t* ready_tasks = create_linked_list_t();
  ready_tasks->deallocator = deallocate_task_t;

  if (list_extract(tasks, ready_tasks, task_is_ready_to_execute) != 0) {
    zlog_error(gc, "task_execute(): Unable to extract ready tasks from linked list");

    free_linked_list_t(ready_tasks);

    return -1;
  }

  it_t it = list_begin(ready_tasks);
  task_t* task = NULL;

  while ((task = (task_t*)it_get(it)) != NULL) {
    if (task->function == NULL || task->function(game) != 0) {
      zlog_warn(gc, "task_execute(): task returned failure");
    }

    it = it_next(it);
  }

  free_linked_list_t(ready_tasks);

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
  task_t* task = (task_t*)value;

  if (time(NULL) > task->execute_at) {
    return 1;
  }

  return 0;
}
