# Create Dynamic Array Utility

Several hot-path linked lists need replacing with contiguous arrays (see tasks 11–14) but the codebase has no dynamic array type.

**Solution:** Add a simple growable pointer array (`ptr_array_t`) to the data structures following existing conventions. Should support push, remove, iteration by index, and an optional deallocator.
