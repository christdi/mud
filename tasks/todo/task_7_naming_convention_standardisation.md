# Naming Convention Standardisation

**Priority:** 07
**Created:** 2026-03-16
**Effort:** High
**Risk:** Medium (wide rename scope — do per subsystem)

## What

Establish and apply a single consistent naming convention for constructor/destructor functions, error return values, and deallocator wrappers across the entire codebase.

## Why

The codebase currently uses at least three different naming schemes for equivalent operations:

| Pattern | Where |
|---|---|
| `create_X_t()` / `free_X_t()` | data structures (`linked_list`, `hash_table`, etc.) |
| `ecs_new_X_t()` / `ecs_free_X_t()` | ECS (`entity`, `component`, `system`, `archetype`) |
| No prefix, just `new_X()` | some lower-level types |

The `_t` suffix on function names is unconventional — in C, `_t` is a naming convention reserved for types (and POSIX reserves identifiers ending in `_t` for future use). Function names using it are misleading.

Error return values are also inconsistent: most functions return `0` for success and `-1` for error, but several in `db.c` return `0` in error paths.

## How

### Agreed convention (confirm with developer before executing)

- Constructor: `mud_X_new()` — allocates and initialises, returns pointer or NULL on failure
- Destructor: `mud_X_free()` — frees; accepts NULL gracefully (like `free()`)
- Error returns: `0` = success, `-1` = error, consistently

Alternatively, if keeping subsystem prefixes is preferred: `game_X_new` / `ecs_X_new` / `net_X_new` etc. The important thing is that the pattern is consistent within and ideally across subsystems.

### Execution approach — do one subsystem per commit

**Do not do this in one large commit.** Each rename should be one focused commit so diffs are reviewable and bisectable.

Suggested order (least to most interconnected):

1. `src/data/linked_list/` — rename `create_linked_list_t` → `linked_list_new`, `free_linked_list_t` → `linked_list_free`, etc.
2. `src/data/hash_table/` — same pattern
3. `src/data/queue/` — same pattern
4. `src/ecs/entity.c` — `ecs_new_entity_t` → `entity_new` etc.
5. `src/ecs/component.c`, `system.c`, `archetype.c`
6. `src/player.c`, `src/task.c`, `src/command.c`, `src/action.c`, `src/event.c`

For each subsystem:
```bash
# Find all usages before renaming
grep -rn "create_linked_list_t\|free_linked_list_t" src/ include/
# Update header first, then implementation, then callers
```

### Error return consistency in `db.c`

Search `src/db.c` for functions that return `0` in error paths where `-1` is the convention:

```
grep -n "return 0" src/db.c
```

Review each return site in context. Where a function returns `0` after an error (failed sqlite step, NULL result, etc.), change to `return -1`. Update callers if they check the return value.

### Verification

After each subsystem rename: `make` in the `build/` directory must produce zero errors and zero new warnings (clang-tidy runs automatically). The rename is purely mechanical so no behavioural change is expected.
