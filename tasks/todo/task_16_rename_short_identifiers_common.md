# Rename Common Short Identifiers: `it`, `rc`, `db`

**Priority:** 16
**Created:** 2026-03-16
**Effort:** Low–Medium (mechanical, widespread)
**Risk:** None
**Warnings eliminated:** ~86 `[readability-identifier-length]`

## What

Rename three high-frequency short identifiers that appear across many files:
- `it_t it` → `it_t iter` (iterator variable — 47 occurrences)
- `it_t it` as a parameter → `it_t iter` (5 occurrences)
- `int rc` / `sqlite3_stmt* rc` → `result` (12 occurrences)
- `sqlite3* db` parameter → `sqlite3* database` (22 occurrences)

## Why

These three names account for 86 warnings and appear across most of the codebase. They are all routine renames with clear, unambiguous replacements and zero semantic change.

## How

### 1. `it` → `iter` (52 occurrences across many files)

`it_t it` is used as the iteration variable in every linked list traversal. Affected files (from build output):

- `src/action.c`
- `src/command.c`
- `src/data/hash_table/hash_table.c`
- `src/data/hash_table/hash_iterator.c`
- `src/data/linked_list/linked_list.c`
- `src/ecs/archetype.c`
- `src/ecs/component.c`
- `src/ecs/system.c`
- `src/event.c`
- `src/player.c`
- `src/task.c`
- `src/lua/game_api.c`
- `src/lua/player_api.c`
- `src/lua/hooks.c`

In each file, replace `it_t it` with `it_t iter`, then replace all uses of the bare `it` variable (as an `it_t` value) with `iter`.

Also update any function prototypes or signatures in headers that include `it_t it` as a named parameter — check `include/mud/data/linked_list/iterator.h` and related headers.

Take care not to rename `it_t` (the type name) — only the variable named `it`.

### 2. `rc` → `result` (12 occurrences)

Used for return codes and SQLite result codes. Affected files:

```
grep -rn "\brc\b" src/
```

Expected in: `src/db.c`, `src/lua/db_api.c`, possibly `src/network/`.

Replace `int rc` / `sqlite3_stmt* rc` declarations and all subsequent uses of `rc` within the same scope.

### 3. `db` → `database` (22 occurrences)

The `sqlite3* db` parameter name in database-related functions. Affected files:

- `src/db.c` — multiple function parameters
- `src/lua/db_api.c` — multiple function parameters

```
grep -rn "\bsqlite3\* db\b\|\bsqlite3\*db\b" src/
grep -rn "\bdb\b" src/db.c src/lua/db_api.c
```

Replace the parameter declaration and all in-scope uses. Note that `db` may also appear as a struct field name (e.g., if `game->db` or similar exists — do not rename struct fields, only local parameters and variables).

Check headers too:
```
grep -rn "\bsqlite3\* db\b" include/
```

### Verification

```bash
make 2>&1 | grep "identifier-length" | grep -E "'it'|'rc'|'db'"
```

Should return zero. Full build must be clean.
