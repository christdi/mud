# Deallocator Wrapper Macro

Every type has an identical `deallocate_x(void*)` wrapper function that exists solely to cast a `void*` and call the real free function. This pattern is repeated ~10 times.

**Solution:** Replace with a macro that generates the wrapper from the type and free function names.
