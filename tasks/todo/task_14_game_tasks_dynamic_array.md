# Replace `game->tasks` Linked List with Sorted Dynamic Array

**Priority:** 14
**Created:** 2026-03-16
**Effort:** Medium
**Risk:** Low–Medium (changes task execution logic)
**Dependencies:** Task 10 (`ptr_array_t` utility)

## What

Replace `linked_list_t* tasks` in `game_t` with a `ptr_array_t*` kept sorted by `task->execute_at`. Update `task_schedule_task`, `task_cancel_task`, and `task_execute_tasks` accordingly.

## Why

`task_execute_tasks` is called every game tick (`src/game.c:187`). Currently it:
1. Allocates a second temporary `linked_list_t` (`ready_tasks`)
2. Calls `list_extract` to scan and move ready tasks into it
3. Iterates `ready_tasks` to execute them
4. Frees the temporary list

This allocates and frees a linked list every tick. With a sorted dynamic array, ready tasks are always at the front (index 0 upward). The tick scan can break as soon as it hits a future task — no temporary list, no allocation, and fewer items examined on average.

`task_cancel_task` (removal by pointer) is O(n) on both structures; cost is unchanged.

## How

### 1. Update `task.h` — `include/mud/task.h`

Replace the `linked_list_t` typedef and all function signatures:

```c
#ifndef MUD_TASK_TASK_H
#define MUD_TASK_TASK_H

#include <time.h>
#include "mud/util/muduuid.h"

typedef struct game     game_t;
typedef struct ptr_array ptr_array_t;
typedef struct lua_ref  lua_ref_t;

typedef struct task {
  mud_uuid_t uuid;
  char*      name;
  time_t     execute_at;
  lua_ref_t* ref;
} task_t;

task_t* task_new_task_t(const char* name, int execute_in, lua_ref_t* ref);
void    task_free_task_t(task_t* task);
void    task_deallocate_task_t(void* value);

int task_schedule_task(ptr_array_t* tasks, task_t* task);
int task_cancel_task(ptr_array_t* tasks, game_t* game, task_t* task);
int task_execute_tasks(ptr_array_t* tasks, game_t* game);

#endif
```

### 2. Update `create_game_t` and `free_game_t` — `src/game.c`

```c
// BEFORE
game->tasks = create_linked_list_t();
game->tasks->deallocator = task_deallocate_task_t;
// ...
free_linked_list_t(game->tasks);

// AFTER
game->tasks = ptr_array_new();
game->tasks->deallocator = task_deallocate_task_t;
// ...
ptr_array_free(game->tasks);
```

### 3. Implement `task_schedule_task` with sorted insertion — `src/task.c`

Insert new tasks in ascending `execute_at` order so that the tasks due soonest are always at the front of the array. This makes `task_execute_tasks` able to break early.

```c
int task_schedule_task(ptr_array_t* tasks, task_t* task) {
  assert(tasks);
  assert(task);

  // Find the insertion point (first task with execute_at > task->execute_at)
  size_t insert_at = tasks->count;
  for (size_t i = 0; i < tasks->count; i++) {
    task_t* existing = (task_t*)tasks->items[i];
    if (existing->execute_at > task->execute_at) {
      insert_at = i;
      break;
    }
  }

  // Grow if needed (reuse ptr_array_push logic, but we need to insert mid-array)
  if (tasks->count == tasks->capacity) {
    size_t new_cap = tasks->capacity * 2;
    void** new_items = realloc(tasks->items, new_cap * sizeof(void*));
    if (!new_items) {
      LOG(ERROR, "task_schedule_task: realloc failed");
      return -1;
    }
    tasks->items    = new_items;
    tasks->capacity = new_cap;
  }

  // Shift items from insert_at onward to make room
  size_t tail = tasks->count - insert_at;
  if (tail > 0) {
    memmove(&tasks->items[insert_at + 1], &tasks->items[insert_at],
            tail * sizeof(void*));
  }

  tasks->items[insert_at] = task;
  tasks->count++;
  return 0;
}
```

Add `#include <string.h>` at the top of `task.c` for `memmove`.

Note: If the sorted insertion is considered too complex and task counts are expected to remain small (<50), an alternative is to use plain `ptr_array_push` and accept an unsorted array. `task_execute_tasks` would then scan all items rather than breaking early. This is simpler to implement and still eliminates the per-tick temporary list allocation.

### 4. Implement `task_execute_tasks` with early exit — `src/task.c`

```c
int task_execute_tasks(ptr_array_t* tasks, game_t* game) {
  assert(tasks);

  time_t now = time(NULL);
  size_t i = 0;

  while (i < tasks->count) {
    task_t* task = (task_t*)tasks->items[i];

    if (task->execute_at > now) {
      break;  // Array is sorted; no later tasks are ready
    }

    lua_call_task_execute_hook(game->lua_state, task);

    // Remove this task: shift remaining items left and free the task
    size_t tail = tasks->count - i - 1;
    if (tail > 0) {
      memmove(&tasks->items[i], &tasks->items[i + 1], tail * sizeof(void*));
    }
    tasks->count--;

    task_free_task_t(task);
    // Do NOT increment i — the item at index i is now the next task
  }

  return 0;
}
```

Note: The task is freed directly here rather than through the deallocator, because we are removing it without going through `ptr_array_remove` (to avoid a second O(n) search). This is correct — no double-free occurs.

### 5. Implement `task_cancel_task` — `src/task.c`

```c
int task_cancel_task(ptr_array_t* tasks, game_t* game, task_t* task) {
  assert(tasks);
  assert(task);

  (void)game;  // game parameter retained for API compatibility; unused

  if (ptr_array_remove(tasks, task) != 0) {
    LOG(ERROR, "task_cancel_task: task not found in task list");
    return -1;
  }

  return 0;
}
```

`ptr_array_remove` calls the deallocator on the item. Since `tasks->deallocator = task_deallocate_task_t`, the task is freed correctly.

### 6. Remove the `task_is_ready_to_execute` predicate

The static predicate function used by `list_extract` is no longer needed. Remove it:

```
grep -n "task_is_ready_to_execute" src/task.c
```

### 7. Update includes in `src/task.c`

Add `#include "mud/data/ptr_array.h"`. Remove `#include "mud/data/linked_list.h"` if no longer used.

### Verification

Build cleanly. Start the game. Schedule several tasks from Lua with different delays (e.g., 1s, 5s, 10s). Confirm they execute at the correct times. Schedule a task and cancel it before it fires. Confirm the cancelled task does not execute. Confirm no memory errors (run under valgrind if available: `valgrind --leak-check=full ./mud`).
