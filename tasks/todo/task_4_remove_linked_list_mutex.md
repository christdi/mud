# Remove `pthread_mutex` from `linked_list_t`

**Priority:** 04
**Created:** 2026-03-16
**Effort:** Medium
**Risk:** Low

## What

Remove the `pthread_mutex_t` field from `linked_list_t` and all associated lock/unlock calls from `list_add`, `list_remove`, and the other list operations in `src/data/linked_list/linked_list.c`.

## Why

The game loop in `src/game.c` is single-threaded — there is no thread creation anywhere in the codebase. Every `list_add` and `list_remove` call currently acquires and releases a mutex unnecessarily. This adds:

- ~40 bytes of wasted memory per list instance (a `pthread_mutex_t` on Linux)
- A lock/unlock overhead on every insert and remove
- Error-handling code paths in `list_add`/`list_remove` for mutex failure that can never meaningfully recover
- Complexity in `init_linked_list` and `free_linked_list_t` for mutex init/destroy

If threading is needed in the future it should be applied at the game-state level, not on every individual list.

## How

### 1. Update the struct — `src/data/linked_list/linked_list.h`

Remove the `pthread_mutex_t mutex` field from `linked_list_t`.

Remove the `#include <pthread.h>` if it is no longer used anywhere in the header.

### 2. Update `src/data/linked_list/linked_list.c`

**`init_linked_list`**: Remove `pthread_mutex_init(&list->mutex, NULL)`.

**`free_linked_list_t`**: Remove `pthread_mutex_destroy(&list->mutex)`.

**`list_add`**: Remove the `pthread_mutex_lock` call, the error-log-and-return block, and the `pthread_mutex_unlock` call with its error block. The function body then just performs the pointer manipulation. Because the error paths around mutex failure currently return `-1`, and callers do not check the return value of `list_add` consistently, removing them simplifies the function to one that cannot fail (unless `calloc` fails — see note below).

**`list_remove`** and any other operations that lock the mutex: apply the same removal.

Remove `#include <errno.h>` and `#include <string.h>` from `linked_list.c` if they were only needed for the mutex error logging.

### 3. Update `CMakeLists.txt`

Check whether `-lpthread` is in `target_link_libraries`. If `pthread` is no longer used anywhere after this change, remove it. Search all `.c` files for remaining `pthread_` usage before removing.

```
grep -rn "pthread_" src/
```

### Note on `calloc` failure in `list_add`

Currently `list_add` calls `node_new()` which calls `calloc`. If `calloc` returns NULL the code will crash on `node->data = value`. This is a pre-existing issue independent of the mutex removal. Optionally add a NULL check on the return of `node_new()` and return `-1` if it fails, making the function's error return meaningful. Callers should then check the return value.

### Verification

Build cleanly (`make` in `build/`) with no new warnings. Run the game and connect a test client to exercise list operations (player connects → commands → disconnects exercises add/remove on the player list).
