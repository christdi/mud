# Collapse Deallocator Wrapper Functions Into a Macro

**Priority:** 09
**Created:** 2026-03-16
**Effort:** Low
**Risk:** None

## What

Replace the repeated `deallocate_X(void*)` wrapper functions with a single macro that generates them.

## Why

The data structures (`linked_list_t`, `hash_table_t`) store `void*` values and call a `void (*deallocator)(void*)` function pointer to free them. Because the actual free functions take typed pointers (`free_player_t(player_t*)`), a small wrapper is needed to do the cast. This wrapper is currently written out by hand for every type:

```c
void deallocate_player(void* value) {
    assert(value);
    player_t* player = (player_t*)value;
    free_player_t(player);
}
```

This pattern repeats ~10 times across the codebase. A macro generates the same code with one line per type.

## How

### 1. Add the macro — `include/mud/data/deallocate.h` (or a new `include/mud/util/macros.h`)

```c
/*
 * Generates a void* deallocator wrapper for use as a data structure
 * deallocator function pointer.
 *
 * Usage: DEFINE_DEALLOCATOR(deallocate_player, player_t, free_player_t)
 */
#define DEFINE_DEALLOCATOR(fn_name, type, free_fn)  \
    void fn_name(void* _v) {                         \
        assert(_v);                                  \
        free_fn((type*)_v);                          \
    }
```

### 2. Replace existing deallocator definitions

Find all existing deallocator functions:

```
grep -rn "^void deallocate_" src/
```

For each one, delete the function body and replace with the macro invocation in the corresponding `.c` file:

```c
// BEFORE
void deallocate_player(void* value) {
    assert(value);
    player_t* player = (player_t*)value;
    free_player_t(player);
}

// AFTER
DEFINE_DEALLOCATOR(deallocate_player, player_t, free_player_t)
```

The function declarations in the corresponding `.h` files remain unchanged — the macro expands to a definition, not a declaration, so callers see no difference.

### 3. Known deallocators to replace

- `deallocate_player` in `src/player.c`
- `deallocate_command` / `deallocate_command_group` in `src/command.c`
- `deallocate_action` in `src/action.c`
- `deallocate_linked_list_t` in `src/data/linked_list/linked_list.c`
- Any ECS deallocators in `src/ecs/`
- Any others found by the grep above

### Verification

`make` must produce zero errors. The macro expands to identical code so no behavioural change is expected. Verify the header include chain is correct (the macro uses `assert`, so the file using the macro must include `<assert.h>`).
