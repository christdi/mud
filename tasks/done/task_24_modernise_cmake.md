---
name: Modernise CMakeLists.txt
description: Fix correctness issues and modernise CMake configuration across both CMakeLists files
type: project
---

## Problem

The CMake configuration has a correctness bug, uses several deprecated commands, and targets a version from 2013. This limits access to modern CMake idioms and produces a less portable build.

## Solution

### Correctness

- `CMAKE_C_STANDARD_REQUIRED` is set without a value (no-op). Set it to `ON`.

### Modernisation

- Bump minimum version to 3.5.
- Remove redundant `enable_language(C)` — `project(mud C)` already enables it.
- Simplify `project()` call — remove the intermediate `set(PROJECT_NAME mud)`.
- Replace `add_definitions("-Wall -pedantic")` with `add_compile_options`, which is the correct command for compiler flags rather than preprocessor defines.
- Replace global `include_directories()` with `target_include_directories()` on each target.
- Replace `find_package(Lua51)` with `find_package(Lua 5.3)`.
- Use `find_package(OpenSSL)` and `find_package(Threads)` to get proper imported targets (`OpenSSL::SSL`, `OpenSSL::Crypto`, `Threads::Threads`) rather than raw library names.
- Replace `file(GLOB_RECURSE)` with an explicit source list for `libmud`.

### tests/CMakeLists.txt

- Introduce a `mud_add_test()` helper function to eliminate the repeated boilerplate for each test target.
- Use `Threads::Threads` imported target instead of raw `pthread`.
