# Replace `game->archetypes` and `game->components` Linked Lists with Dynamic Arrays

**Priority:** 13
**Created:** 2026-03-16
**Effort:** Medium
**Risk:** Low–Medium (signature change propagates to component.c and Lua API)
**Dependencies:** Task 10 (`ptr_array_t`), Task 11 (archetype->components inline array)

## What

Replace `linked_list_t* archetypes` and `linked_list_t* components` in `game_t` with `ptr_array_t*`. Update all call sites, including the signature of `ecs_update_entity_archetypes` and `ecs_remove_entity_from_all_components` which currently accept `linked_list_t*` directly.

## Why

`game->archetypes` is iterated inside `ecs_update_entity_archetypes` every time any entity gains or loses a component (triggered via `ecs_add_entity_to_component` and `ecs_remove_entity_from_component` in `component.c`). This is the outer loop of the ECS hot path (task 11 addresses the inner loop). Converting to a `ptr_array_t` removes per-node pointer chasing from this traversal.

`game->components` is iterated in `ecs_remove_entity_from_all_components`, called when an entity is deleted. Although this is a cold path, converting it is low-effort once the archetype change is done, and maintains consistency.

## How

### 1. Update `game_t` — `include/mud/game.h`

```c
typedef struct game {
  // ...
  ptr_array_t* components;   // was linked_list_t*
  ptr_array_t* archetypes;   // was linked_list_t*
  ptr_array_t* systems;      // already changed in task 12
  // ...
} game_t;
```

Ensure `ptr_array_t` is forward-declared in this header (may already be present from task 12).

### 2. Update `create_game_t` — `src/game.c`

```c
// BEFORE
game->components = create_linked_list_t();
game->components->deallocator = ecs_deallocate_component_t;

game->archetypes = create_linked_list_t();
game->archetypes->deallocator = ecs_deallocate_archetype_t;

// AFTER
game->components = ptr_array_new();
game->components->deallocator = ecs_deallocate_component_t;

game->archetypes = ptr_array_new();
game->archetypes->deallocator = ecs_deallocate_archetype_t;
```

### 3. Update `free_game_t` — `src/game.c`

```c
// BEFORE
free_linked_list_t(game->components);
free_linked_list_t(game->archetypes);

// AFTER
ptr_array_free(game->components);
ptr_array_free(game->archetypes);
```

### 4. Update `ecs_update_entity_archetypes` — `src/ecs/archetype.c` and `archetype.h`

This function's signature currently takes `linked_list_t* archetypes`. Update it to take `ptr_array_t*`.

**`include/mud/ecs/archetype.h`:**
```c
// Add forward declaration
typedef struct ptr_array ptr_array_t;

// Change signature
void ecs_update_entity_archetypes(ptr_array_t* archetypes, entity_t* entity);
```

**`src/ecs/archetype.c`:**
```c
// BEFORE
void ecs_update_entity_archetypes(linked_list_t* archetypes, entity_t* entity) {
  it_t it = list_begin(archetypes);
  archetype_t* archetype = NULL;

  while ((archetype = it_get(it)) != NULL) {
    if (ecs_entity_matches_archetype(archetype, entity)) {
      if (!ecs_archetype_has_entity(archetype, entity)) {
        ecs_add_entity_to_archetype(archetype, entity);
      }
    } else {
      if (ecs_archetype_has_entity(archetype, entity)) {
        ecs_remove_entity_from_archetype(archetype, entity);
      }
    }
    it = it_next(it);
  }
}

// AFTER
void ecs_update_entity_archetypes(ptr_array_t* archetypes, entity_t* entity) {
  for (size_t i = 0; i < archetypes->count; i++) {
    archetype_t* archetype = (archetype_t*)archetypes->items[i];

    if (ecs_entity_matches_archetype(archetype, entity)) {
      if (!ecs_archetype_has_entity(archetype, entity)) {
        ecs_add_entity_to_archetype(archetype, entity);
      }
    } else {
      if (ecs_archetype_has_entity(archetype, entity)) {
        ecs_remove_entity_from_archetype(archetype, entity);
      }
    }
  }
}
```

### 5. Update `ecs_remove_entity_from_all_components` — `src/ecs/component.c` and `component.h`

**`include/mud/ecs/component.h`:**
```c
// Change signatures for both functions that take linked_list_t* for components/archetypes
void ecs_add_entity_to_component(component_t* component, component_data_t* data,
                                  ptr_array_t* archetypes, entity_t* entity);
void ecs_remove_entity_from_component(component_t* component,
                                       ptr_array_t* archetypes, entity_t* entity);
void ecs_remove_entity_from_all_components(ptr_array_t* components,
                                            ptr_array_t* archetypes, entity_t* entity);
```

**`src/ecs/component.c`** — update `ecs_remove_entity_from_all_components`:
```c
// BEFORE
void ecs_remove_entity_from_all_components(linked_list_t* components,
                                            linked_list_t* archetypes, entity_t* entity) {
  it_t it = list_begin(components);
  component_t* component = NULL;

  while ((component = it_get(it)) != NULL) {
    ecs_remove_entity_from_component(component, archetypes, entity);
    it = it_next(it);
  }
}

// AFTER
void ecs_remove_entity_from_all_components(ptr_array_t* components,
                                            ptr_array_t* archetypes, entity_t* entity) {
  for (size_t i = 0; i < components->count; i++) {
    component_t* component = (component_t*)components->items[i];
    ecs_remove_entity_from_component(component, archetypes, entity);
  }
}
```

The other two functions (`ecs_add_entity_to_component`, `ecs_remove_entity_from_component`) already pass `archetypes` through to `ecs_update_entity_archetypes` — update their parameter type from `linked_list_t*` to `ptr_array_t*` and their call to `ecs_update_entity_archetypes` remains the same (just with the new type).

### 6. Update Lua API call sites — `src/lua/game_api.c`

Find all sites that add to or remove from `game->archetypes` or `game->components`:

```
grep -n "game->archetypes\|game->components" src/lua/game_api.c
```

Replace `list_add` with `ptr_array_push`. There are no mid-runtime removals of archetypes or components expected, but if `list_remove` appears, replace with `ptr_array_remove`.

### 7. Update `ecs_remove_entity_from_all_components` call site

The call in `src/ecs/entity.c` (entity deletion) passes `game->components` and `game->archetypes`:

```
grep -n "ecs_remove_entity_from_all_components" src/
```

No change to the logic, just the type of the arguments now matches `ptr_array_t*`.

### 8. Remove unused includes

After changes, remove `#include "mud/data/linked_list.h"` from any file that no longer uses `linked_list_t`. Add `#include "mud/data/ptr_array.h"` wherever needed.

### Verification

Build with zero warnings (clang-tidy runs automatically). Start the game. From Lua, register archetypes and components, create entities, add/remove components, and verify archetype membership updates correctly via `game.get_archetype_entities` and `game.matches_archetype`. Delete an entity and confirm no component or archetype state is left dangling.
