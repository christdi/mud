# Replace Archetype Component List with Inline Array

Each archetype stores its component list as a heap-allocated linked list. This list is iterated on every entity component change (the innermost loop of the ECS hot path) and never changes after setup.

**Solution:** Replace the linked list with a fixed inline array in the archetype struct. Eliminates the heap allocation and reduces the hot-path traversal to a plain loop over contiguous memory.
