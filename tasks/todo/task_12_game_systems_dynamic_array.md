# Replace `game->systems` Linked List with Dynamic Array

**Priority:** 12
**Created:** 2026-03-16
**Effort:** Small–Medium
**Risk:** Low
**Dependencies:** Task 10 (`ptr_array_t` utility must exist first)

## What

Replace the `linked_list_t* systems` field in `game_t` with a `ptr_array_t* systems`, updating all sites that add, remove, or iterate systems.

## Why

`game->systems` is iterated on every game tick via `ecs_update_systems` (`src/ecs/system.c:84`). This is called unconditionally in the main loop (`src/game.c:188`). With a linked list, each iteration step dereferences a separately-allocated `node_t` pointer before reaching the `system_t`. A `ptr_array_t` stores all system pointers contiguously so the loop walks a single flat allocation — no intermediate node dereferences.

Systems are registered at startup (or occasionally at runtime from Lua) and almost never removed. The remove operation is O(n) with a dynamic array, which is acceptable for a collection that stays small and static.

## How

### 1. Update `game_t` — `include/mud/game.h`

```c
// Add forward declaration / include for ptr_array_t
typedef struct ptr_array ptr_array_t;

typedef struct game {
  // ... existing fields ...
  ptr_array_t* components;   // was linked_list_t*
  ptr_array_t* archetypes;   // was linked_list_t*  (done in task 13)
  ptr_array_t* systems;      // was linked_list_t*
  // tasks and events remain for now
  // ...
} game_t;
```

Change only `systems` as part of this task. `components` and `archetypes` are handled in task 13. Keep `linked_list_t* tasks` and `linked_list_t* events` unchanged.

### 2. Update `create_game_t` — `src/game.c`

```c
// BEFORE
game->systems = create_linked_list_t();
game->systems->deallocator = ecs_deallocate_system_t;

// AFTER
game->systems = ptr_array_new();
game->systems->deallocator = ecs_deallocate_system_t;
```

### 3. Update `free_game_t` — `src/game.c`

```c
// BEFORE
free_linked_list_t(game->systems);

// AFTER
ptr_array_free(game->systems);
```

### 4. Update `ecs_update_systems` — `src/ecs/system.c`

Change the signature of `ecs_update_systems` and update `include/mud/ecs/system.h` to match.

```c
// BEFORE (system.h)
void ecs_update_systems(game_t* game);

// AFTER — no signature change needed; game_t is passed and systems accessed from it
```

The implementation changes from linked list iteration to a plain for loop:

```c
// BEFORE
void ecs_update_systems(game_t* game) {
  it_t it = list_begin(game->systems);
  system_t* system = NULL;

  while ((system = it_get(it)) != NULL) {
    if (system->enabled) {
      if (lua_call_system_execute_hook(game->lua_state, system) == -1) {
        LOG(ERROR, "Failed to execute system");
      }
    }
    it = it_next(it);
  }
}

// AFTER
void ecs_update_systems(game_t* game) {
  for (size_t i = 0; i < game->systems->count; i++) {
    system_t* system = (system_t*)game->systems->items[i];

    if (system->enabled) {
      if (lua_call_system_execute_hook(game->lua_state, system) == -1) {
        LOG(ERROR, "Failed to execute system");
      }
    }
  }
}
```

Add `#include "mud/data/ptr_array.h"` to `system.c`.

### 5. Update Lua API — `src/lua/game_api.c`

Find all sites in `game_api.c` that add to or remove from `game->systems` (register/deregister system calls). Replace `list_add` / `list_remove` with `ptr_array_push` / `ptr_array_remove`:

```c
// BEFORE
list_add(game->systems, system);
list_remove(game->systems, system);

// AFTER
ptr_array_push(game->systems, system);
ptr_array_remove(game->systems, system);
```

Search `src/lua/game_api.c` for `game->systems` to find all call sites.

### 6. Update includes

In `src/game.c` and `src/ecs/system.c`, add:
```c
#include "mud/data/ptr_array.h"
```

If `linked_list.h` is no longer referenced in these files after the change, remove its include.

### Verification

Build cleanly. Start the server and confirm that systems registered from Lua execute each tick. Register multiple systems; deregister one; confirm remaining systems still execute. The behaviour is identical — only the internal iteration mechanism changes.
