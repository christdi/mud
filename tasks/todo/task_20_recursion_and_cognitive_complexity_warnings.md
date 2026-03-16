# Address Recursion and Cognitive Complexity Warnings

**Priority:** 20
**Created:** 2026-03-16
**Effort:** Medium
**Risk:** Low
**Warnings eliminated:** 12 (`misc-no-recursion` × 7, `readability-function-cognitive-complexity` × 5)

## What

Suppress legitimate `misc-no-recursion` warnings on naturally recursive functions, and reduce cognitive complexity in flagged functions where practical.

## Why

The JSON parser and Lua/JSON bridge are recursive by design — a recursive-descent parser for a recursive data format is the idiomatic approach. Flagging these as warnings is a false positive. The high cognitive complexity warnings on the same functions and on `network_telnet_on_input` are more meaningful and benefit from refactoring.

## How

### Part A — Suppress `misc-no-recursion` (7 warnings, zero code change needed)

These functions are correctly recursive. Add `// NOLINT(misc-no-recursion)` to each function definition:

| File | Function |
|------|----------|
| `src/json.c` | `json_free_json_node_t` |
| `src/json.c` | `parse_value` |
| `src/json.c` | `parse_object` |
| `src/json.c` | `parse_array` |
| `src/json.c` | `write_node_value` |
| `src/lua/struct.c` | `lua_push_json_value` |
| `src/lua/struct.c` | `lua_to_json_node` |

Place the comment on the line of the function definition:

```c
// NOLINT(misc-no-recursion)
static json_node_t* parse_object(const char* input, size_t len, int* pos) {
```

Alternatively, disable this check globally in `CMakeLists.txt` by adding `-misc-no-recursion` to the `-checks=` string. This is appropriate if recursion will be used in future code as well.

### Part B — Reduce cognitive complexity (5 warnings)

#### `write_node_value` in `src/json.c` — complexity 65 (threshold 25)

This function was already improved in task 01 (boolean simplification, single snprintf). If the complexity is still above threshold after those changes, the remaining complexity comes from the `OBJECT` and `ARRAY` cases which contain nested loops and conditionals.

Extract the object and array serialisation into helper functions:

```c
static int write_object_value(json_node_t* node, char* buffer, size_t len, size_t* pos);
static int write_array_value(json_node_t* node, char* buffer, size_t len, size_t* pos);
```

`write_node_value` then delegates to these for the `OBJECT` and `ARRAY` cases, reducing its own complexity substantially.

#### `parse_object` in `src/json.c` — complexity 54 (threshold 25)

`parse_object` is a state machine with ~10 states, each with multiple branches. Extract the state-transition logic for each state into a small inline or static helper, or split by state group (key-parsing vs value-parsing vs separator-parsing).

#### `parse_array` in `src/json.c` — complexity 40 (threshold 25)

Similar to `parse_object` — extract the inner per-character dispatch into a helper function or simplify the state machine.

#### `lua_to_json_node` in `src/lua/struct.c` — complexity 31 (threshold 25)

This function converts a Lua value on the stack to a `json_node_t`. It switches on Lua type with nested logic. Extract each Lua type handler into a small static function:

```c
static json_node_t* json_node_from_lua_string(lua_State* lua, int stack_idx);
static json_node_t* json_node_from_lua_number(lua_State* lua, int stack_idx);
static json_node_t* json_node_from_lua_table(lua_State* lua, int stack_idx);
```

#### `network_telnet_on_input` in `src/network/telnet.c` — complexity 27 (threshold 25)

A telnet option negotiation state machine. Extract the per-byte handling logic for each state into a dedicated static function, or simplify by consolidating similar option codes.

#### `player_get_commands` in `src/player.c` — complexity 26 (threshold 25)

Marginally over threshold. This function iterates command groups and matches commands by name. Extract the inner command-matching loop into a helper:

```c
static command_t* find_command_in_group(command_group_t* group,
                                         hash_table_t* commands,
                                         const char* name);
```

### Verification

```bash
make 2>&1 | grep "misc-no-recursion\|cognitive-complexity"
```

Should return zero results after Part A suppressions and Part B refactoring. All extracted functions should be `static` (file-local). Build must remain clean with no new warnings.
