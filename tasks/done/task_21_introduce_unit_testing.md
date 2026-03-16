---
name: Introduce unit testing
description: Add a unit test suite using Unity, backed by a static library CMake build
type: project
---

## Problem

There are no unit tests. Refactoring tasks have no automated safety net and regressions can only be caught by manual testing or at runtime.

## Solution

### Framework

Vendor **Unity** (single `.c`/`.h`, no external dependencies) into `tests/vendor/`. No install required.

### CMake restructuring

Split the current single-target build into:
- `libmud` — static library compiled from all `src/` files except `main.c`
- `mud` executable — links `libmud`, provides `main()`
- `test_mud` executable — links `libmud` and all test sources, provides its own `main()` via Unity

This avoids `main()` conflicts and means source files are compiled once and shared.

### Single test executable

All test suites compile into one binary (`test_mud`). Each test file exposes a `run_X_tests()` function. A top-level `test_runner.c` calls them all. One `ctest` entry runs the full suite.

### Directory structure

```
tests/
  vendor/           # Unity source (unity.c, unity.h, unity_internals.h)
  data/
    test_linked_list.c
    test_hash_table.c
  test_runner.c     # main(), calls all run_X_tests()
  CMakeLists.txt
```

### Vertical slice: `linked_list.c`

First test file covers: `list_add`, `list_remove`, `list_steal`, `list_size`, `list_contains`, `list_clear`, `list_extract`, and iterator traversal. This also acts as a template for subsequent test files.

### clang-tidy

Exclude `tests/` from clang-tidy to avoid noise from Unity macros.

### Progression

After linked_list: `hash_table.c`, `json.c`. Modules with external dependencies (Lua, SQLite) can be added later using CMocka for mocking.
