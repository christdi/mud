# Use Designated Initialisers for Struct Construction

**Priority:** 08
**Created:** 2026-03-16
**Effort:** Low (apply incrementally)
**Risk:** None

## What

Where constructor functions initialise struct fields individually after a `calloc`, use C99 designated initialisers to make the non-zero fields explicit and the intent clearer.

## Why

The current pattern is:
```c
player_t* p = calloc(1, sizeof *p);
p->socket = -1;
p->state = STATE_NEW;
// (username, command_groups left as NULL by calloc)
```

This requires reading every following line to know the full initial state. With a compound literal assignment after `calloc`, all non-zero initial values are visible at a glance:

```c
player_t* p = calloc(1, sizeof *p);
if (!p) return NULL;
*p = (player_t){ .socket = -1, .state = STATE_NEW };
```

Fields not listed in the designated initialiser are zero-initialised by the compound literal, consistent with what `calloc` provides.

## How

This is an incremental change — apply it whenever a constructor function is being touched for another reason, rather than as a dedicated sweep.

### Pattern to apply

```c
// BEFORE
foo_t* f = calloc(1, sizeof *f);
f->field_a = VALUE_A;
f->field_b = VALUE_B;
// field_c left NULL/0 by calloc
return f;

// AFTER
foo_t* f = calloc(1, sizeof *f);
if (!f) return NULL;
*p = (foo_t){ .field_a = VALUE_A, .field_b = VALUE_B };
return f;
```

Note: the `if (!f) return NULL` NULL check is worth adding at the same time. `calloc` can return NULL if allocation fails and currently these failures are silently ignored in most constructors.

### Files where this applies most clearly

- `src/player.c` — `create_player_t`
- `src/task.c` — `create_task_t`
- `src/command.c` — `create_command_t`, `create_command_group_t`
- `src/action.c` — `create_action_t`
- `src/event.c` — any event allocation
- `src/ecs/entity.c`, `component.c`, `system.c`

### Do not apply where

- The struct has many fields all left at their zero values — `calloc` already handles this cleanly.
- The initialisation logic is conditional or involves non-trivial expressions better left as sequential statements.
