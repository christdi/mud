# Rename Remaining Short Identifiers

After tasks 15 and 16, ~50 identifier-length warnings remain from context-specific short names: loop counters (`i`, `j`), parser state (`p`), network/socket parameters (`fd`, `ps`, `op`), and others.

`src/bsd/string.c` is a verbatim BSD implementation and should be excluded from clang-tidy rather than modified.

**Solution:** Rename each identifier to a descriptive name based on context. Add a per-directory clang-tidy configuration to exclude the BSD source file.
