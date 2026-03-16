# Remove Redundant NULL Guards Before `free`

**Priority:** 02
**Created:** 2026-03-16
**Effort:** Low
**Risk:** None

## What

Remove all `if (ptr != NULL) { free(ptr); }` patterns throughout the codebase, replacing them with a direct `free(ptr)` call.

## Why

The C standard (C11 §7.22.3.3) guarantees that `free(NULL)` is a no-op. The NULL guards add visual noise, make cleanup code look more complex than it is, and create a false impression that `free(NULL)` would be dangerous. Removing them reduces line count and makes teardown functions easier to read.

## How

Search for the pattern across all `.c` files:

```
grep -rn "if (.*!= NULL)" src/
grep -rn "if (.*)" src/ | grep free
```

For each match, check if the body is solely a `free()` or a `free_X_t()` call. If so, unwrap it.

### Known locations

**`src/player.c:50-52`**
```c
// BEFORE
if (player->username != NULL) {
    free(player->username);
}

// AFTER
free(player->username);
```

**`src/player.c:54-56`**
```c
// BEFORE
if (player->command_groups != NULL) {
    free_linked_list_t(player->command_groups);
}

// AFTER
free_linked_list_t(player->command_groups);
```

Note: `free_linked_list_t` currently asserts its argument is non-null (`assert(list)` on line 37 of `linked_list.c`). Before removing the guard here, either:
- Add a NULL early-return to `free_linked_list_t` before the assert, or
- Keep the guard only for functions that assert non-null internally.

The same consideration applies anywhere a custom `free_X_t` function is called conditionally — check whether that function has an `assert` at the top. If it does, either update the assert to a guard (`if (!x) return;`) or keep the NULL check at the call site. Prefer updating the free functions to handle NULL gracefully, which is the more conventional C pattern.

Apply consistently across `src/game.c`, `src/event.c`, and any other files where the pattern appears.
