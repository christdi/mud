# Flatten Hash Node Allocations

Every hash table insert allocates twice: once for the node struct and once for the key string via `strdup`. These two objects always share a lifetime.

**Solution:** Combine into a single allocation using a flexible array member to store the key inline in the node struct. Halves allocator calls per insert and improves locality.
