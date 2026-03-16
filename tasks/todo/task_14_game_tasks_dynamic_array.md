# Replace Tasks Linked List with Sorted Dynamic Array

`game->tasks` is scanned every tick to find ready tasks, and currently allocates a temporary list each tick to extract them into.

**Solution:** Replace with a `ptr_array_t` (task 10) kept sorted by execution time. Ready tasks are always at the front, allowing early exit and removing the need for a temporary list on each tick.
