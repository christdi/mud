# Remove Mutex from Linked List

Every linked list instance carries a `pthread_mutex_t` and every insert/remove acquires and releases it. The game loop is single-threaded — this overhead serves no purpose.

**Solution:** Remove the mutex and all associated lock/unlock logic from the linked list. If thread safety is needed in future it belongs at a higher level, not on every list instance.
