# Recursion and Cognitive Complexity Warnings

clang-tidy flags several JSON parsing and Lua/JSON bridge functions for being recursive, and flags others for exceeding the cognitive complexity threshold.

The recursion warnings are false positives — recursive-descent parsers for a recursive data format are correct by design. The complexity warnings on the parser and Lua conversion functions are genuine and benefit from refactoring.

**Solution:** Suppress `misc-no-recursion` on the naturally recursive functions. Reduce complexity in flagged functions by extracting per-type or per-state logic into smaller helper functions.
