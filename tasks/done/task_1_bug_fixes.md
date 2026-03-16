# Bug Fixes

**Status:** Done

## Off-by-one writes in JSON parser

Two allocation sites in the JSON parser wrote a null terminator one byte past the end of their allocation, causing undefined behaviour.

**Solution:** Write the null terminator at index `len`, not `len + 1`.

## Unsafe `sprintf` in JSON serialiser

The serialiser measured required buffer space with `snprintf(NULL, 0, ...)` then wrote with an unbounded `sprintf`, bypassing the bounds check.

**Solution:** Replace each pair with a single `snprintf` call that both checks bounds and writes. Simplify the boolean branch from two `if` blocks to a ternary.

## Hash key truncation bug

Keys were stored via `strdup` with no length limit, but hashing and comparison only considered the first `MAX_KEY_LENGTH` characters. Two keys sharing the same prefix would be treated as identical.

The original truncation guard intended to fix this but could never trigger due to a logic error in its condition.

**Solution:** Replace `strdup` with `strndup(key, MAX_KEY_LENGTH)` so the stored key is always bounded consistently with hashing and comparison.
