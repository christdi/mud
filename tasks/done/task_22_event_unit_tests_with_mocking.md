---
name: Event unit tests with link-seam mocking
description: Demonstrate testing a module with dependencies by adding tests for event.c
type: project
---

## Problem

`event.c` has external dependencies (`player_on_event`) that prevent it from being tested in isolation using the same pure approach as `linked_list.c`. There is no established pattern for testing modules with dependencies.

## Solution

Use **link-seam mocking**: compile a substitute `tests/mocks/mock_player.c` into the test binary in place of the real `player.c`. The mock records invocations (call count, last-seen pointers) allowing assertions without a real player implementation.

The `test_event` binary is a separate executable from `test_mud` because it links different object files (mock instead of real). Both are registered with `ctest` and run together. Still follows the Unity pattern: each test file exposes `run_X_tests()`, a runner provides `main()`.

### Files needed to link `test_event`

Only the minimal transitive dependencies of `event.c`:
- `src/event.c` — module under test
- `src/log.c` — required by the LOG macro
- `src/data/linked_list/` — used internally by event broker and hash table
- `src/data/hash_table/` — used by `event_dispatch_events`
- `src/data/queue/` — used by `event_submit_event` / dispatch loop
- `tests/mocks/mock_player.c` — replaces `player.c`

No Lua, SQLite, or game dependencies required.

### Tests to cover

Without mocking: `event_new_event_t`, `event_free_event_t` (deallocator called / not called), `event_new_event_broker_t`, `event_free_event_broker_t`, `event_has_events`, `event_submit_event`.

With mocking: `event_dispatch_events` — verify `player_on_event` is called once per player per event, and that events are dequeued after dispatch.

### Future

CMocka would be the next step if argument-level verification macros (`expect_value`, `check_expected`) or stricter call-count enforcement are needed. The link-seam approach is sufficient for call-count and pointer verification.
