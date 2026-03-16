# JSON Serialiser Refactor

The JSON serialiser measures required buffer space with one `snprintf` call then writes with a second, making every write a redundant double-evaluation of the format string.

**Solution:** Collapse each pair into a single `snprintf` call. `snprintf` returns the would-be length, so one call both performs the bounds check and writes.
