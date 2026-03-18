# Event-Driven Poll Loop

## Problem

The game loop runs all subsystems — input polling, event dispatch, ECS updates, task execution, and output flushing — in lockstep at a fixed tick rate (default 20 TPS, 50ms/tick). This means player input is not read until the next tick and responses are not sent until the end of that tick, introducing up to a full tick of latency for every interaction, including commands that have no game-world effect at all.

## Solution

Decouple the loop rate from the game logic rate.

The main loop runs at a high poll rate (default 100 Hz, 10ms/iteration) and does only three things each iteration: poll for input, execute any due tasks, flush output. Event dispatch and ECS updates are registered as recurring engine tasks that fire at the configured `ticks_per_second` cadence.

This mirrors the Node.js event loop model: I/O is handled as fast as possible; scheduled/periodic work runs at its own cadence via the task scheduler.

### Changes

- **Task system:** Switch `execute_at` from second-resolution (`time_t`) to microsecond-resolution (`struct timeval`). Add `interval_ms` for recurring tasks. Add a C function callback field so engine-internal tasks can be registered without going through Lua.
- **Config:** Add `poll_rate` (Hz) controlling the main loop frequency. `ticks_per_second` retains its meaning as the game logic cadence.
- **Game loop:** Remove `event_dispatch_events` and `ecs_update_systems` from the loop body. Register them as recurring engine tasks at startup. Replace `sleep_until_tick` with `sleep_poll_interval` using `poll_rate`.
- **Lua task API:** `schedule_task` now accepts fractional seconds (e.g. `0.5`) for sub-second one-shot tasks. New `schedule_recurring_task` schedules a repeating task. `lib/task.lua` gains a corresponding `recurring` constructor.
