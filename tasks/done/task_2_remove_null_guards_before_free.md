# Remove Null Guards Before `free`

`free(NULL)` is a no-op per the C standard. Null checks before `free` calls are unnecessary noise.

**Solution:** Remove `if (ptr != NULL)` guards that wrap a single `free` or custom free function call. Custom free functions that assert non-null will need those asserts softened to handle NULL gracefully first.
