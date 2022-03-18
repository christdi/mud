#include "lua.h"
#include "lauxlib.h"

#include "mud/lua/ref.h"

/**
 * Releases a Lua reference
 *
 * l - Lua state to release reference against
 * ref - Reference to be released
**/
void lua_release_ref(lua_State* l, int ref) {
  luaL_unref(l, LUA_REGISTRYINDEX, ref);
}
