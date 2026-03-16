# Rename `lua_State* l` Parameter

Every Lua API callback uses `l` as its `lua_State*` parameter name, generating ~157 clang-tidy identifier-length warnings — the largest single warning category in the build.

**Solution:** Rename to `lua` across all Lua binding files.
