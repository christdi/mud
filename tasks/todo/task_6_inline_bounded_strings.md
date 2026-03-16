# Inline Bounded Strings Into Owner Structs

**Priority:** 06
**Created:** 2026-03-16
**Effort:** Medium
**Risk:** Low–Medium

## What

Replace heap-allocated `char*` fields (set via `strdup`) with fixed `char[]` arrays for strings whose maximum length is known at compile time and whose lifetime is tied to the owning struct.

## Why

Several structs store strings as `char*` fields populated by `strdup`. This means every struct creation does an extra allocation, every struct teardown needs a conditional free (or just a free), and there is always a risk of forgetting the free. For strings with a bounded size (UUIDs are always exactly 36 characters; usernames have a configurable max), an inline array eliminates the separate allocation entirely, makes the struct self-contained, and removes a class of potential leak.

## How

### Priority targets

**UUID strings** — always exactly 36 characters plus a null terminator (37 bytes). Any struct storing a UUID as `char* uuid` set via `strdup(uuid_string)` is a candidate. Affected structs likely include `command_t`, `action_t`, and others. Check by searching:

```
grep -rn "char\* uuid" include/
grep -rn "strdup.*uuid" src/
```

Replace with:
```c
// BEFORE
char* uuid;
// set via: s->uuid = strdup(uuid_str(...));
// freed via: free(s->uuid);

// AFTER
#define UUID_STR_LEN 36
char uuid[UUID_STR_LEN + 1];
// set via: strlcpy(s->uuid, uuid_str(...), sizeof s->uuid);
// freed: nothing, freed with the struct
```

**`task_t.name`** — task names are developer-defined identifiers. Determine the maximum expected length, add a `#define TASK_NAME_MAX_LEN` constant (64 is reasonable), and change:
```c
// BEFORE (src/task.c and include/mud/task.h)
char* name;
// set via: t->name = strdup(name);
// freed via: free(t->name);

// AFTER
char name[TASK_NAME_MAX_LEN + 1];
// set via: strlcpy(t->name, name, sizeof t->name);
```

**`player_t.username`** — if a `MAX_USERNAME_LEN` constant already exists or can be defined, apply the same transformation. Check `src/player.c` and `include/mud/player.h`.

### Steps for each field

1. Update the header to change `char* field` → `char field[MAX_LEN + 1]` and add or reference the relevant max-length constant.
2. Update the constructor (`create_X_t` or `new_X`) to use `strlcpy(s->field, src, sizeof s->field)` instead of `strdup`.
3. Update the destructor (`free_X_t`) to remove the `free(s->field)` call.
4. Search for all other places where the field is assigned (not just in the constructor) and update those to `strlcpy` as well.

```
grep -rn "->uuid\s*=" src/
grep -rn "->name\s*=" src/
grep -rn "->username\s*=" src/
```

### Caution

Do not apply this to strings that are genuinely variable-length and potentially large (e.g., player input buffers, SQL query strings, JSON output). Only apply where an upper bound is natural and tight. If the right max length is unclear, leave the field as `char*` rather than picking an arbitrary size.
