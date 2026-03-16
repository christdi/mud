# Rename Common Short Identifiers

Several high-frequency short identifiers trigger clang-tidy identifier-length warnings across many files: `it` (iterator, ~52 warnings), `db` (database handle, ~22 warnings), `rc` (return code, ~12 warnings).

**Solution:** Rename to `iter`, `database`, and `result` respectively throughout the codebase.
