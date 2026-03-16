# Rename `lua_State* l` Parameter to `lua_State* lua`

**Priority:** 15
**Created:** 2026-03-16
**Effort:** Low (mechanical rename)
**Risk:** None
**Warnings eliminated:** ~157 `[readability-identifier-length]`

## What

Rename the `lua_State* l` parameter to `lua_State* lua` across all Lua binding files in `src/lua/`.

## Why

Every Lua API callback function uses `l` as its `lua_State*` parameter name. This triggers 157 `readability-identifier-length` warnings from clang-tidy (the single largest warning category in the build). The name `lua` is unambiguous, requires no change to semantics, and makes the parameter immediately recognisable.

## How

The rename is mechanical. Every affected file has the same pattern: a function declared `static int lua_something(lua_State* l)` with uses of `l` throughout the body.

### Files to update

Run this search to confirm all call sites:
```
grep -rn "\blua_State\* l\b\|[^a-z]l\b" src/lua/
```

**Files affected** (confirmed from build output):
- `src/lua/common.c`
- `src/lua/db_api.c`
- `src/lua/game_api.c`
- `src/lua/hooks.c`
- `src/lua/log_api.c`
- `src/lua/player_api.c`
- `src/lua/script_api.c`
- `src/lua/struct.c`
- `src/config.c` (one instance: a local `lua_State* l` variable, not a parameter — rename to `lua_state`)

### Approach

For each file, do a careful rename of `l` → `lua` **only** where it refers to the `lua_State*`. Do not blindly replace all `l` — the letter appears in other contexts (e.g., string literals, unrelated variables).

The safest approach per-file:
1. Search for `lua_State\* l[^a-z]` to find parameter declarations — rename to `lua_State* lua`
2. Search for `\bl\b` (word-boundary match) to find usages within function bodies — rename each to `lua`
3. Take care not to match `l` in variable names like `val`, `level`, `list`, etc.

Using a structured editor or IDE refactor (rename symbol) per function is the safest method. Alternatively, sed with word-boundary matching:

```bash
# Example for one file — review diff carefully before applying
sed -i 's/lua_State\* l)/lua_State* lua)/g; s/\bl\b/lua/g' src/lua/game_api.c
```

This sed approach is risky due to false positives on single-letter `l`. **Review every diff** before committing.

### Verification

```bash
make 2>&1 | grep "identifier-length" | grep "'l'"
```

Should return zero results. Build must produce zero errors.
