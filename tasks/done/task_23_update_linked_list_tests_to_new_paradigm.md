---
name: Update linked list tests to match current test paradigm
description: Add scenario comments to test_linked_list.c to match the convention established in test_event.c
type: project
---

## Problem

`tests/data/test_linked_list.c` was written before the current test conventions were established. It is missing the short scenario comment (≤ 3 lines) required before each test function declaration.

## Solution

Add a one-to-three line comment above each test function describing the scenario under test, matching the style used in `tests/event/test_event.c`.
