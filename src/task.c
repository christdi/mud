#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "mud/data/linked_list.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/hooks.h"
#include "mud/lua/ref.h"
#include "mud/task.h"

static void on_task_timer(uv_timer_t* timer);
static void on_task_close(uv_handle_t* handle);

/**
 * Allocates a new task_t and initialises iter.  The task is not scheduled until
 * task_schedule_task is called.
 *
 * Returns the newly allocated task_t.
 **/
task_t* task_new_task_t(game_t* game, const char* name, int execute_in, lua_ref_t* ref) {
  task_t* task = calloc(1, sizeof(task_t));

  task->uuid = new_uuid();
  task->name = strdup(name);
  task->execute_in = execute_in;
  task->game = game;
  task->ref = ref;

  return task;
}

/**
 * Frees an allocated task_t.  The uv timer must be closed before this is called.
 **/
void task_free_task_t(task_t* task) {
  assert(task);

  free(task->name);
  lua_free_lua_ref_t(task->ref);

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
 * Adds a task to the task list and starts its libuv timer.
 *
 * Parameters
 *  game - game instance owning the task list and event loop
 *  task - the task to be scheduled
 *
 * Returns 0 on success or -1 on failure
 **/
int task_schedule_task(game_t* game, task_t* task) {
  assert(game);
  assert(task);

  int res = uv_timer_init(game->loop, &task->timer);

  if (res != 0) {
    LOG(ERROR, "uv_timer_init failed for task [%s]: %s", task->name, uv_strerror(res));

    return -1;
  }

  task->timer.data = task;

  uint64_t timeout_ms = (uint64_t)task->execute_in * 1000;

  res = uv_timer_start(&task->timer, on_task_timer, timeout_ms, 0);

  if (res != 0) {
    LOG(ERROR, "uv_timer_start failed for task [%s]: %s", task->name, uv_strerror(res));

    return -1;
  }

  if (list_add(game->tasks, task) != 0) {
    LOG(ERROR, "Unable to add task [%s] to task list", task->name);
    uv_timer_stop(&task->timer);

    return -1;
  }

  return 0;
}

/**
 * Cancels a task that is pending execution by stopping its timer.
 *
 * game - game instance owning the task list
 * task - task to be cancelled
 *
 * Returns 0 on success
 **/
int task_cancel_task(game_t* game, task_t* task) {
  assert(game);
  assert(task);

  uv_timer_stop(&task->timer);
  uv_close((uv_handle_t*)&task->timer, on_task_close);

  return 0;
}

/**
 * Closes all pending task timers during engine shutdown.
 *
 * game - game instance owning the task list
 **/
void task_shutdown(game_t* game) {
  assert(game);

  it_t iter = list_begin(game->tasks);
  task_t* task = NULL;

  while ((task = (task_t*)it_get(iter)) != NULL) {
    iter = it_next(iter);

    if (!uv_is_closing((uv_handle_t*)&task->timer)) {
      uv_timer_stop(&task->timer);
      uv_close((uv_handle_t*)&task->timer, on_task_close);
    }
  }
}

/**
 * Called by libuv when a task timer fires.  Executes the Lua callback then
 * closes the timer to trigger cleanup.
 **/
static void on_task_timer(uv_timer_t* timer) {
  task_t* task = timer->data;

  lua_call_task_execute_hook(task->game->lua_state, task);

  uv_close((uv_handle_t*)timer, on_task_close);
}

/**
 * Called by libuv after a task timer handle is closed.  Removes the task from
 * the game task list and frees iter.
 **/
static void on_task_close(uv_handle_t* handle) {
  task_t* task = handle->data;

  list_remove(task->game->tasks, task);
  task_free_task_t(task);
}
