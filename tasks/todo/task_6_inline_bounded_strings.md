# Inline Bounded Strings Into Owner Structs

Several structs store short strings as heap-allocated `char*` fields via `strdup`, despite having a natural maximum length. This adds an allocation per struct creation and a corresponding free in teardown.

**Solution:** Replace `char*` fields with fixed inline arrays where an upper bound is obvious and tight (UUID fields, task names, player usernames). Eliminates the separate allocation and the possibility of forgetting the free.
