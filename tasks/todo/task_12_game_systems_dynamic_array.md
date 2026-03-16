# Replace Systems Linked List with Dynamic Array

`game->systems` is iterated on every game tick. Each iteration step dereferences a separately allocated linked list node before reaching the system, causing unnecessary pointer chasing.

**Solution:** Replace with a `ptr_array_t` (task 10). Systems are registered at startup and rarely removed, making a flat array the natural fit.
