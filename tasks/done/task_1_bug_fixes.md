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

### 3. Dead truncation logic and key storage bug in `hash_table.c`

The original code had a dead truncation block whose intent was valid but whose implementation was broken:

```c
char* hash_key = strdup(key);
size_t len = strnlen(hash_key, MAX_KEY_LENGTH - 1);   // caps at MAX_KEY_LENGTH-1

if (len > MAX_KEY_LENGTH) {    // can never be true — len is at most MAX_KEY_LENGTH-1
    LOG(ERROR, ...);
    hash_key[MAX_KEY_LENGTH] = '\0';
}
```

Removing the dead block revealed a deeper correctness issue: `strdup` stores the full key with no length bound, but all lookup operations (`hash_table_get`, `hash_table_has`, `hash_table_delete`) compare using `strncmp(node->key, key, MAX_KEY_LENGTH)`. The hash function also only hashes the first `MAX_KEY_LENGTH` characters. This means two keys that share the same first `MAX_KEY_LENGTH` characters but differ beyond that position would hash to the same bucket and be considered identical — incorrect behaviour.

The original truncation block was attempting to prevent this by capping the stored key, but it never fired. The correct fix is to use `strndup` instead of `strdup`:

```c
// BEFORE
char* hash_key = strdup(key);

// AFTER
char* hash_key = strndup(key, MAX_KEY_LENGTH);
```

`strndup` copies at most `MAX_KEY_LENGTH` characters and always null-terminates, so the stored key is always bounded to `MAX_KEY_LENGTH` characters. The `strncmp` in lookups then performs a full comparison of the stored key, and behaviour is consistent with how the hash function treats the key.

**Note:** In practice all keys in this codebase are UUID strings (36 characters, well under `MAX_KEY_LENGTH` of 50), so this bug would not manifest. The fix is nonetheless correct and removes a latent footgun for any future longer keys.
