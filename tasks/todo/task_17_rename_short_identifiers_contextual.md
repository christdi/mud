# Rename Remaining Short Identifiers and Exclude Third-Party Code

**Priority:** 17
**Created:** 2026-03-16
**Effort:** Low–Medium
**Risk:** None
**Warnings eliminated:** ~50 `[readability-identifier-length]`

## What

Address the remaining scattered short identifier warnings not covered in task 16. Also exclude `src/bsd/string.c` from clang-tidy since it is third-party BSD code with intentionally terse variable names.

## Why

After tasks 15 and 16 there are still ~50 identifier-length warnings spread across loop counters (`i`, `j`), parser state variables (`p`), network/socket parameters (`fd`, `ps`, `op`), and single-character variables in utility code. Individually small, but eliminating them completes the identifier cleanup.

## How

### 1. Exclude `src/bsd/string.c` from clang-tidy

`src/bsd/string.c` is a verbatim BSD `strlcpy`/`strlcat` implementation with intentionally short variable names (`d`, `s`, `n`, `s1`, `s2`, `c1`, `c2`). These should not be changed. Exclude it from clang-tidy in `CMakeLists.txt` by updating the `-header-filter` and adding a source exclusion.

The cleanest approach is to add a `.clang-tidy` file in `src/bsd/` that disables all checks:

**Create `src/bsd/.clang-tidy`:**
```yaml
---
Checks: '-*'
...
```

clang-tidy respects per-directory configuration files. This suppresses all warnings for files in that directory without touching `CMakeLists.txt`.

### 2. Rename `i` and `j` loop counters → `idx` and `jdx`

Loop counters warned about (from build output):
- `src/data/hash_table/hash_table.c` — loop over hash buckets
- `src/json.c` — character position loop counters (5 instances)
- `src/log.c` — loop in log formatting
- `src/lua/common.c` — loop counter
- `src/network/client.c` — loop counter
- `src/util/mudhash.c` — hex digest loop
- `src/util/mudstring.c` — string processing loops (2 instances)

In each case, rename `i` → `idx` and `j` → `jdx`. These are always simple `for` loop counters; update the declaration and all three loop clauses.

### 3. Rename context-specific short names

These appear in specific files only. Identify and rename each:

**`p` in `src/json.c`** (parser state variable, 3 occurrences):
```
grep -n "\bp\b" src/json.c
```
Rename to `state` or `parser_state`. This is the state machine variable tracking parse progress in `parse_object` and `parse_array`.

**`ps` in `src/network/`** (poll set / `struct pollfd`, 2+4=6 occurrences):
```
grep -rn "\bps\b" src/network/
```
Rename to `poll_set` or `poll_fds`.

**`fd` as a parameter** (file descriptor, 4 occurrences in network code):
```
grep -rn "int fd\b\|int\* fd\b" src/network/
```
Rename to `sock_fd` or `file_fd` depending on context.

**`op` in telnet/network** (telnet option code, 2 occurrences):
```
grep -rn "\bop\b" src/network/
```
Rename to `option` or `telnet_option`.

**`in` parameter** (2 occurrences — likely an input parameter):
```
grep -rn "\bin\b" src/
```
Note: `in` is a reserved keyword in C++ and best avoided. Rename to `input` or `data_in`.

**`w` variable** (2 occurrences):
```
grep -rn "\bw\b" src/
```
Rename based on context (likely a write buffer or width).

**`c` in hash/string utilities** (7 occurrences):
```
grep -rn "\bc\b" src/data/ src/util/ src/network/
```
Rename to `byte_val` in hash function, `chr` or `ch` in string processing contexts (though clang-tidy requires 3+ characters, so use `chr` or `cur_char`).

**`se`, `sb`, `tm` in `src/log.c`** (1 each):
```
grep -n "\bse\b\|\bsb\b\|\btm\b" src/log.c
```
`tm` is the `struct tm*` from `localtime` — rename to `time_info`. `se`/`sb` — rename based on what they represent.

### 4. Remaining single-letter parameters (`s1`, `s2`, `c`)

In network or string comparison functions. Rename to descriptive names:
- `s1`, `s2` → `str_a`, `str_b` or `left`, `right`
- `c` as a parameter → `chr` or `delim` based on context

### Verification

```bash
make 2>&1 | grep "identifier-length"
```

After tasks 15, 16, and 17 this should return zero results (except possibly any inside `src/bsd/` which will be suppressed by the directory-level `.clang-tidy`).
