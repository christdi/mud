# `json.c` Serializer: Collapse Double-`snprintf` Pattern

**Priority:** 03
**Created:** 2026-03-16
**Effort:** Medium
**Risk:** Low

## What

Rewrite the value-serialization switch in `src/json.c` to eliminate the two-call `snprintf`-then-`sprintf` pattern, replacing each pair with a single `snprintf` call. Also simplify the redundant boolean branch.

## Why

The current pattern calls `snprintf` twice for every value written: once with a NULL buffer to measure the required length, then again (as an unsafe `sprintf`) to actually write. This is unnecessary — `snprintf` already writes to the buffer and returns the would-be length, so the two operations can always be merged into one. The redundant calls double the work and the unsafe `sprintf` calls undermine the bounds check that precedes them.

Additionally, the `BOOLEAN` case checks `== true` and then separately checks `== false`, with no `else`. This is needlessly verbose for a two-state value.

## How

### The general pattern replacement

Every branch in the switch (approximately lines 722–855 of `src/json.c`) currently looks like:

```c
// BEFORE
if ((val_len = snprintf(NULL, 0, fmt, arg)) > len - *pos) {
    return -1;
}
sprintf(buffer + *pos, fmt, arg);
*pos += val_len;

// AFTER
val_len = snprintf(buffer + *pos, len - *pos, fmt, arg);
if (val_len < 0 || (size_t)val_len >= len - *pos) {
    return -1;
}
*pos += val_len;
```

The `snprintf` return value semantics: returns the number of characters that *would* have been written excluding the null terminator. If this value is >= the buffer size passed, the output was truncated (or there was no room). Checking `>= len - *pos` catches both truncation and the edge case where the write would exactly fill the remaining space (leaving no room for a null terminator that downstream code might depend on).

Apply this replacement to all branches: `STRING`, `NUMBER`, `BOOLEAN`, `NIL`, `OBJECT` key serialization, `ARRAY` serialization, and any other call sites. Search for `sprintf(buffer` in `src/json.c` to find every instance.

### BOOLEAN simplification

```c
// BEFORE
case BOOLEAN:
    if (node->value->boolean == true) {
        // snprintf + sprintf block for TRUE_STR
        break;
    }
    if (node->value->boolean == false) {
        // snprintf + sprintf block for FALSE_STR
        break;
    }
    break;

// AFTER
case BOOLEAN: {
    const char* bool_str = node->value->boolean ? TRUE_STR : FALSE_STR;
    val_len = snprintf(buffer + *pos, len - *pos, "%s", bool_str);
    if (val_len < 0 || (size_t)val_len >= len - *pos) {
        return -1;
    }
    *pos += val_len;
    break;
}
```

### Verification

After the change, run a round-trip test: parse a known JSON string, serialize it back, and compare to the original. The existing `dist/` game scripts that use JSON push/pull via Lua can serve as a functional smoke test. Connect a client and exercise any Lua code that calls `json.serialize`.
