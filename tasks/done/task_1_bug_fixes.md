# Bug Fixes

**Priority:** 01
**Created:** 2026-03-16
**Effort:** Low
**Risk:** None

## What

Fix three confirmed bugs: two off-by-one writes in `json.c` and ~11 uses of unsafe `sprintf` in the serializer, plus dead/broken key-truncation logic in `hash_table.c`.

## Why

These are correctness issues, not style. The off-by-one writes one byte past the end of their allocation (undefined behaviour). The `sprintf` calls bypass the bounds check that precedes them, making buffer overflows possible if the check logic ever changes. The truncation guard in `hash_table.c` can never trigger, leaving misleading dead code.

## How

### 1. Off-by-one in `json.c` (2 instances)

**`src/json.c:366`**

```c
// BEFORE
key_buffer = calloc(1, key_len + 1);
memcpy(key_buffer, start, key_len);
key_buffer[key_len + 1] = '\0';   // writes one byte past the allocation

// AFTER
key_buffer = calloc(1, key_len + 1);
memcpy(key_buffer, start, key_len);
key_buffer[key_len] = '\0';
```

A duplicate of this same bug exists around line 568 in the `parse_string` function. Apply the same fix there.

### 2. Unsafe `sprintf` in `json.c` serializer (~11 call sites, lines 728–855)

Every branch in the `switch` statement uses this two-step pattern:

```c
// BEFORE
val_len = snprintf(NULL, 0, fmt, args);      // measure
if (val_len > len - *pos) return -1;         // bounds check
sprintf(buffer + *pos, fmt, args);            // unsafe write
*pos += val_len;

// AFTER
val_len = snprintf(buffer + *pos, len - *pos, fmt, args);
if (val_len < 0 || (size_t)val_len >= len - *pos) return -1;
*pos += val_len;
```

`snprintf` returns the number of characters that *would* have been written. If that exceeds the remaining capacity it means nothing useful was written, so we return -1. This replaces the unsafe `sprintf` and removes the redundant first `snprintf` call in one step.

Affected lines (approximate): 724–728, 735–739, 747–751, 759–763, 773–777, 784–788, 793–797, 806–810, 816–820, 827–831, 841–845. Search for `sprintf(buffer` in `src/json.c` to find all instances.

### 3. Dead truncation logic in `hash_table.c:73-79`

```c
// BEFORE
char* hash_key = strdup(key);
size_t len = strnlen(hash_key, MAX_KEY_LENGTH - 1);   // caps at MAX_KEY_LENGTH-1

if (len > MAX_KEY_LENGTH) {    // can never be true
    LOG(ERROR, ...);
    hash_key[MAX_KEY_LENGTH] = '\0';
}

// AFTER
char* hash_key = strdup(key);
// No truncation block needed; strnlen already caps the hash computation.
// If key length validation is desired, check before calling insert.
```

Remove lines 75–79 (the `if (len > MAX_KEY_LENGTH)` block). The `len` variable is still used by `get_hash_index` implicitly via the loop — verify `get_hash_index` uses its own `strnlen` call (it does, at line 45) so the local `len` variable after the `strdup` is unused and can be removed too.
