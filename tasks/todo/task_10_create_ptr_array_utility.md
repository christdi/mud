# Create `ptr_array_t` Dynamic Array Utility

**Priority:** 10
**Created:** 2026-03-16
**Effort:** Small
**Risk:** None (new code, no existing code changed)
**Prerequisite for:** Tasks 12, 13, 14

## What

Add a simple dynamic (growable) pointer array to `src/data/` following the same conventions as the existing `linked_list` and `hash_table` data structures. This will be used to replace `linked_list_t` wherever sequential, index-accessible storage is needed.

## Why

Three hot-path linked lists (`game->systems`, `game->archetypes`, `game->tasks`) are iterated every tick or on every component change. A contiguous array of pointers allows the CPU prefetcher to work effectively — all pointers are in a single allocation that can be walked with a plain `for` loop, with no indirection through separately allocated `node_t` structs.

The existing `linked_list_t` stays in place for the event queue (FIFO) where it is the correct choice.

## How

### Files to create

**`include/mud/data/ptr_array.h`**

```c
#ifndef MUD_DATA_PTR_ARRAY_H
#define MUD_DATA_PTR_ARRAY_H

#include <stddef.h>

typedef struct ptr_array {
  void**  items;
  size_t  count;
  size_t  capacity;
  void  (*deallocator)(void*);
} ptr_array_t;

ptr_array_t* ptr_array_new(void);
void         ptr_array_free(ptr_array_t* arr);

int  ptr_array_push(ptr_array_t* arr, void* item);
int  ptr_array_remove(ptr_array_t* arr, void* item);

#endif
```

**`src/data/ptr_array/ptr_array.c`**

```c
#include "mud/data/ptr_array.h"
#include "mud/log.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define PTR_ARRAY_INITIAL_CAPACITY 8

ptr_array_t* ptr_array_new(void) {
  ptr_array_t* arr = calloc(1, sizeof *arr);
  if (!arr) return NULL;

  arr->items = calloc(PTR_ARRAY_INITIAL_CAPACITY, sizeof(void*));
  if (!arr->items) { free(arr); return NULL; }

  arr->capacity = PTR_ARRAY_INITIAL_CAPACITY;
  return arr;
}

void ptr_array_free(ptr_array_t* arr) {
  if (!arr) return;

  if (arr->deallocator) {
    for (size_t i = 0; i < arr->count; i++) {
      arr->deallocator(arr->items[i]);
    }
  }

  free(arr->items);
  free(arr);
}

/* Append an item, growing the backing buffer by doubling if necessary.
 * Returns 0 on success, -1 on allocation failure. */
int ptr_array_push(ptr_array_t* arr, void* item) {
  assert(arr);
  assert(item);

  if (arr->count == arr->capacity) {
    size_t new_cap = arr->capacity * 2;
    void** new_items = realloc(arr->items, new_cap * sizeof(void*));

    if (!new_items) {
      LOG(ERROR, "ptr_array_push: realloc failed growing to capacity %zu", new_cap);
      return -1;
    }

    arr->items    = new_items;
    arr->capacity = new_cap;
  }

  arr->items[arr->count++] = item;
  return 0;
}

/* Remove the first occurrence of item (by pointer equality).
 * Calls the deallocator if one is set.
 * Remaining items are shifted left to fill the gap — O(n) but arrays are small.
 * Returns 0 if found and removed, -1 if not found. */
int ptr_array_remove(ptr_array_t* arr, void* item) {
  assert(arr);
  assert(item);

  for (size_t i = 0; i < arr->count; i++) {
    if (arr->items[i] == item) {
      if (arr->deallocator) {
        arr->deallocator(arr->items[i]);
      }

      /* Shift everything after i left by one */
      size_t tail = arr->count - i - 1;
      if (tail > 0) {
        memmove(&arr->items[i], &arr->items[i + 1], tail * sizeof(void*));
      }

      arr->count--;
      return 0;
    }
  }

  return -1;
}
```

### Iteration pattern

Callers iterate using a plain `for` loop — no iterator type needed:

```c
for (size_t i = 0; i < arr->count; i++) {
  my_type_t* item = (my_type_t*)arr->items[i];
  // use item
}
```

### Add to CMakeLists.txt

`CMakeLists.txt` uses `file(GLOB_RECURSE SRC_FILES ...)` so the new `.c` file is picked up automatically. No changes needed.

### Verification

Build cleanly. Unit-test manually by registering a system and confirming it executes each tick. The three consumer tasks (12, 13, 14) provide the real integration test.
