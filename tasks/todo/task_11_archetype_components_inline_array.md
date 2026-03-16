# Replace `archetype->components` Linked List with Inline Fixed Array

**Priority:** 11
**Created:** 2026-03-16
**Effort:** Small–Medium
**Risk:** Low
**Dependencies:** None (self-contained to `archetype.h` / `archetype.c`)

## What

Replace the `linked_list_t* components` field in `archetype_t` with a fixed-size inline array of `component_t*` pointers and an integer count. Remove the heap allocation for this list entirely.

## Why

`archetype->components` is iterated inside `ecs_entity_matches_archetype`, which is called from `ecs_update_entity_archetypes`, which is called every time any entity gains or loses a component. It is the innermost loop of the ECS hot path:

```
component add/remove
  → ecs_update_entity_archetypes (iterates game->archetypes)
      → ecs_entity_matches_archetype (iterates archetype->components)
          → ecs_component_has_entity (hash table lookup)
```

The list is always small (2–5 components per archetype), never changes after the archetype is registered, and is never iterated except in this one function. A fixed inline array eliminates the heap allocation for the list and all of its nodes, removes the `it_t` iterator machinery from the hot loop, and reduces the function to a plain `for` loop over contiguous memory.

## How

### 1. Define a capacity constant and update the struct — `include/mud/ecs/archetype.h`

```c
#define MAX_ARCHETYPE_COMPONENTS 16

typedef struct archetype {
  hash_table_t* entities;
  component_t*  components[MAX_ARCHETYPE_COMPONENTS];
  int           component_count;
} archetype_t;
```

Remove the forward declaration of `linked_list_t` from this header — it is no longer needed. Remove the `#include` or typedef for `linked_list_t` if it has no other use in the file.

Update the signature of `ecs_update_entity_archetypes` — it currently takes `linked_list_t* archetypes` because the archetypes themselves were stored in a linked list. This will change in task 13; leave the signature as-is for now and update it as part of task 13.

### 2. Update `ecs_new_archetype_t` — `src/ecs/archetype.c`

```c
// BEFORE
archetype_t* ecs_new_archetype_t() {
  archetype_t* archetype = calloc(1, sizeof(archetype_t));
  archetype->components = create_linked_list_t();
  archetype->entities = create_hash_table_t();
  return archetype;
}

// AFTER
archetype_t* ecs_new_archetype_t() {
  archetype_t* archetype = calloc(1, sizeof *archetype);
  if (!archetype) return NULL;
  archetype->entities = create_hash_table_t();
  // components array and component_count are zero-initialised by calloc
  return archetype;
}
```

### 3. Update `ecs_free_archetype_t` — `src/ecs/archetype.c`

```c
// BEFORE
void ecs_free_archetype_t(archetype_t* archetype) {
  assert(archetype);
  free_linked_list_t(archetype->components);
  free_hash_table_t(archetype->entities);
  free(archetype);
}

// AFTER
void ecs_free_archetype_t(archetype_t* archetype) {
  assert(archetype);
  // components are not owned by the archetype — they are pointers to
  // component_t instances owned by game->components. Do not free them.
  free_hash_table_t(archetype->entities);
  free(archetype);
}
```

### 4. Update `ecs_add_archetype_component` — `src/ecs/archetype.c`

```c
// BEFORE
void ecs_add_archetype_component(archetype_t* archetype, component_t* component) {
  list_add(archetype->components, component);
}

// AFTER
void ecs_add_archetype_component(archetype_t* archetype, component_t* component) {
  assert(archetype);
  assert(component);
  assert(archetype->component_count < MAX_ARCHETYPE_COMPONENTS);

  archetype->components[archetype->component_count++] = component;
}
```

### 5. Update `ecs_remove_archetype_component` — `src/ecs/archetype.c`

```c
// BEFORE
void ecs_remove_archetype_component(archetype_t* archetype, component_t* component) {
  list_remove(archetype->components, component);
}

// AFTER
void ecs_remove_archetype_component(archetype_t* archetype, component_t* component) {
  assert(archetype);
  assert(component);

  for (int i = 0; i < archetype->component_count; i++) {
    if (archetype->components[i] == component) {
      // Shift remaining pointers left
      int tail = archetype->component_count - i - 1;
      if (tail > 0) {
        memmove(&archetype->components[i], &archetype->components[i + 1],
                tail * sizeof(component_t*));
      }
      archetype->component_count--;
      return;
    }
  }
}
```

Add `#include <string.h>` at the top of `archetype.c` for `memmove`.

### 6. Update `ecs_entity_matches_archetype` — `src/ecs/archetype.c`

```c
// BEFORE
bool ecs_entity_matches_archetype(archetype_t* archetype, entity_t* entity) {
  it_t it = list_begin(archetype->components);
  component_t* component = NULL;

  while ((component = it_get(it)) != NULL) {
    if (!ecs_component_has_entity(component, entity)) {
      return false;
    }
    it = it_next(it);
  }
  return true;
}

// AFTER
bool ecs_entity_matches_archetype(archetype_t* archetype, entity_t* entity) {
  for (int i = 0; i < archetype->component_count; i++) {
    if (!ecs_component_has_entity(archetype->components[i], entity)) {
      return false;
    }
  }
  return true;
}
```

### 7. Remove unused includes from `archetype.c`

Remove `#include "mud/data/linked_list.h"` if it is no longer referenced anywhere in the file after these changes.

### Verification

Build cleanly. Start the game, register an archetype from Lua, add and remove components on an entity, and confirm the archetype membership updates correctly via `game.get_archetype_entities`.
