# Fix `concurrency-mt-unsafe` Warnings

**Priority:** 18
**Created:** 2026-03-16
**Effort:** Low–Medium
**Risk:** Low
**Warnings eliminated:** ~31 `[concurrency-mt-unsafe]`

## What

Resolve `concurrency-mt-unsafe` warnings by replacing non-thread-safe C library calls with their `_r` (reentrant) equivalents where possible, and suppressing the check for the handful of call sites where no safe alternative exists (legitimately single-threaded contexts).

## Why

clang-tidy flags several C standard library functions as not thread-safe. Most of these warnings are in `src/data/linked_list/linked_list.c` (`strerror` in mutex error paths) — those will disappear automatically when task 04 (remove mutex from linked_list) is completed. The remaining warnings are real and should be fixed.

## Note on linked_list.c warnings

**Do not fix these** as part of this task. The 16–17 warnings from `linked_list.c` (lines 74, 94, 115, 133, etc.) are all `strerror` calls inside mutex error-handling code. Task 04 removes the mutex and all associated error paths entirely, eliminating those warnings. Fix the others first; confirm linked_list.c warnings are gone after task 04.

## How

### 1. `localtime` → `localtime_r` in `src/log.c:42`

`localtime` returns a pointer to a shared static buffer — not thread-safe.

```c
// BEFORE (log.c:42)
struct tm* tm = localtime(&current_time);

// AFTER
struct tm time_info;
localtime_r(&current_time, &time_info);
```

Update the `strftime` call on the next line to use `&time_info` instead of `tm`. Also rename the variable from `tm` to `time_info` (this also fixes the `identifier-length` warning for `tm` from task 17).

`localtime_r` is POSIX — already available on Linux (the only supported platform per README).

### 2. `strerror` → `strerror_r` in network files

`strerror` uses a shared buffer. Replace with `strerror_r` which writes into a caller-provided buffer.

Affected call sites:
- `src/network/server.c` — lines 76, 84, 90, 96, 102, 128, 134, 153
- `src/network/client.c` — lines 103, 151, 181
- `src/network/network.c` — line 185
- `src/game.c` — line 134 (if this is a `strerror` call; confirm)

For each site the pattern is `LOG(ERROR, "%s", strerror(errno))`. Replace with:

```c
// BEFORE
LOG(ERROR, "%s", strerror(errno));

// AFTER
char err_buf[128];
strerror_r(errno, err_buf, sizeof err_buf);
LOG(ERROR, "%s", err_buf);
```

If `strerror_r` appears many times in the same file, define a helper macro at the top of the file to avoid repeating the buffer declaration:

```c
#define LOG_ERRNO() do { \
    char _err_buf[128];  \
    strerror_r(errno, _err_buf, sizeof _err_buf); \
    LOG(ERROR, "%s", _err_buf); \
} while(0)
```

Then replace `LOG(ERROR, "%s", strerror(errno));` with `LOG_ERRNO();` throughout the file.

Note: POSIX specifies two versions of `strerror_r` (GNU and XSI). On Linux with glibc and `_GNU_SOURCE` defined, the GNU version is used (returns `char*` rather than `int`). Check which version is active by looking at existing `#define` directives or testing. If the GNU version: `char* msg = strerror_r(errno, buf, sizeof buf); LOG(ERROR, "%s", msg);`.

### 3. Suppress `getopt` in `src/config.c:66`

`getopt` has no thread-safe equivalent in POSIX. The config parsing runs at startup before any game logic and is inherently single-threaded. Suppress the warning with a NOLINT comment:

```c
// NOLINT(concurrency-mt-unsafe)
while ((opt = getopt(argc, argv, ":s:l:d:p:t:h")) != -1) {
```

### 4. Suppress `exit` in `src/game.c:134`

`exit()` is flagged because it calls atexit handlers which may not be thread-safe. In a single-threaded process this is not a concern. Suppress:

```c
exit(-1); // NOLINT(concurrency-mt-unsafe)
```

### Verification

After completing this task and task 04:
```bash
make 2>&1 | grep "concurrency-mt-unsafe"
```
Should return zero results.
