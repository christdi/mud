# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

An unnamed MUD (Multi-User Dungeon) game framework written in C with Lua scripting. The engine handles networking, persistence, and core game mechanics; games are written in Lua using the provided API.

## Build

```bash
mkdir -p build && cd build
cmake ..
make
```

Run from the project root (config.lua is read from the working directory):
```bash
./mud
```

**Dependencies:** Lua 5.3, SQLite3, OpenSSL (libssl/libcrypto), libuuid

clang-tidy runs automatically during builds with a broad set of checks.

To run the test suite (from `build/`):
```bash
ctest --output-on-failure
```

## Configuration

`config.lua` (project root) controls runtime settings:
- `game_script` — entry point for the game Lua code (default: `dist/main.lua`)
- `lib_script` — engine Lua library (default: `lib/main.lua`)
- `game_port` — TCP port (default: 5000)
- `database_file` — SQLite database path
- `ticks_per_second` — game loop rate (default: 5)

## Architecture

### Layered design

```
TCP/IP clients
     ↓
Network layer  (src/network/ — client.c, server.c, telnet.c, gmcp.c, protocol.c)
     ↓
Game loop      (src/game.c  — poll input → dispatch events → run tasks → update ECS systems → flush output)
     ↓
ECS            (src/ecs/    — entity, component, archetype, system)
Event system   (src/event.c — broker-based event dispatch)
Task scheduler (src/task.c  — scheduled/deferred task execution)
     ↓
Lua scripting  (src/lua/    — C bindings exposed to Lua)
     ↓
SQLite3        (src/db.c    — persistence)
```

### Lua integration

The C engine exposes ~9 Lua binding modules (`src/lua/`): `game_api`, `player_api`, `db_api`, `log_api`, `script_api`, `hooks`, `common`, `struct`, `ref`.

`lib/` contains a higher-level Lua library built on top of those bindings, providing abstractions for archetypes, components, entities, events, narrators, players, state machines, systems, and tasks.

`dist/` is the sample game — shows how a real game is structured using `lib/`. Its subdirectories map to engine concepts: `action/`, `command/`, `component/`, `entity/`, `event/`, `narrator/`, `state/`, `system/`.

### Key files

| File | Role |
|------|------|
| `src/game.c` | Main game loop |
| `src/main.c` | Entry point, startup |
| `src/player.c` | Player state, I/O buffering |
| `src/db.c` | SQLite ORM-like layer |
| `src/json.c` | JSON serialization/deserialization |
| `src/command.c` + `src/action.c` | Player command dispatch |
| `src/event.c` | Event broker |
| `src/task.c` | Task scheduler |
| `include/mud/` | All public headers |

## Code Style

`.clang-format` enforces WebKit-based style: 2-space indentation, attached braces. Run `clang-format` on changed files before committing.

## Communication Style

Be concise. No preamble, no trailing summaries of what was just done. Commit messages should explain why, not restate what the diff already shows.

## Task Tracking

Planned and in-progress work is tracked in `tasks/`. Each task is a markdown file named `task_<N>_<description>.md`. Completed tasks move from `tasks/todo/` to `tasks/done/`.

Tasks are Jira-style tickets: state the problem and the proposed solution at a high level. No file paths, line numbers, or code examples — implementation detail is left to whoever actions the ticket.

## Standard Task Workflow

When actioning a task:
1. Create a task file in `tasks/todo/` if one doesn't exist.
2. Create a new git branch for the work.
3. Make changes. Commit at logical intervals — not all at once at the end.
4. Verify: ensure the build compiles cleanly and `ctest` passes.
5. Update `CLAUDE.md` with any context worth preserving across sessions.
6. Move the task file from `tasks/todo/` to `tasks/done/`.
7. Push the branch for PR review.

## Testing

Tests live in `tests/`. The framework is **Unity v2.5.2**, vendored in `tests/vendor/`. All test binaries are registered with CTest and run via `ctest`.

Each test file is a self-contained binary: it defines `setUp()`, `tearDown()`, and `main()` directly. There is no shared runner. Each binary is registered with `ctest` independently so `ctest` still runs everything in one command.

Test binaries do **not** link `libmud`. `tests/CMakeLists.txt` lists only the source files the module under test actually needs. This keeps each binary minimal and explicit.

### Pure logic tests (no mocking)

Add a `tests/<area>/test_<module>.c` file. Include `unity.h`, write test functions, add `setUp`/`tearDown`/`main`. Add an `add_executable` + `add_test` block to `tests/CMakeLists.txt` with only the required source files. See `tests/data/test_linked_list.c` as the reference.

### Tests with dependencies (FFF mocking)

The mocking framework is **FFF (Fake Function Framework)**, vendored as `tests/vendor/fff.h` (single header). There are no separate mock files — fakes are declared inline at the top of the test file:

```c
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(player_on_event, player_t*, game_t*, event_t*);
```

FFF's generated fake satisfies the missing symbol without linking the real implementation. Reset a fake between tests with `RESET_FAKE(player_on_event)`. Assert using the generated struct: `player_on_event_fake.call_count`, `player_on_event_fake.arg0_history[0]`, etc.

See `tests/event/test_event.c` as the reference implementation.

### Test file conventions

- Short comment (≤ 3 lines) before each test function describing the scenario.
- Each test function is self-contained: set up, act, assert, clean up — no reliance on setUp/tearDown for state.
- `setUp()` and `tearDown()` are defined in the runner and left empty.

## Important Code Notes

**Single-threaded:** The game loop (`src/game.c`) is single-threaded. There is no thread creation in the codebase. `concurrency-mt-unsafe` warnings from clang-tidy are largely false positives; see `tasks/todo/task_18_fix_thread_safety_warnings.md` for the plan to address them.

**Third-party code:** `src/bsd/string.c` is a verbatim BSD `strlcpy`/`strlcat` implementation. Do not modify it or treat its short variable names as a style issue.

**clang-tidy warning volume:** A clean build currently produces ~343 warnings, predominantly `readability-identifier-length`. These are tracked and being addressed incrementally via tasks 15–20. New code should not introduce additional warnings.
