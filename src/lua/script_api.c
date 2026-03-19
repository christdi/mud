#include "lauxlib.h"
#include "lua.h"

#include "mud/data/hash_table.h"
#include "mud/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/script.h"
#include "mud/lua/script_api.h"
#include "mud/util/muduuid.h"

#define SCRIPT_LIB_NAME "script"

static int lua_script_available(lua_State* lua);

static const struct luaL_Reg script_lib[] = {
  { "available", lua_script_available },
  { NULL, NULL }
};

/**
 * Registers the script API functions.
 * 
 * lua - The Lua state.
 * 
 * Returns 0 on success
 **/
int lua_script_register_api(lua_State* lua) {
  lua_push_api_table(lua);
  
  lua_pushstring(lua, SCRIPT_LIB_NAME);
  luaL_newlib(lua, script_lib);
  
  lua_rawset(lua, -3);

  return 0;

  return 0;
}

/**
 * Lua API method for retrieving all available scripts.
 * 
 * lua - The Lua state.
 * 
 * Returns the table on the stack or calls luaL_error on error.
 **/
static int lua_script_available(lua_State* lua) {
  game_t* game = lua_get_game(lua);

  linked_list_t* scripts = create_linked_list_t();
  scripts->deallocator = deallocate_script;

  if (db_script_load_all(game->database, scripts) == -1) {
    LOG(ERROR, "Error loading scripts from database");

    return luaL_error(lua, "Error loading scripts from database");
  }

  script_t* script = NULL;
  it_t iter = list_begin(scripts);
  int index = 1;

  lua_newtable(lua); // stack = 1 table

  while ((script = it_get(iter)) != NULL) {
    linked_list_t* groups = create_linked_list_t();
    groups->deallocator = script_deallocate_script_group_t;

    if (db_script_sandbox_group_by_script_id(game->database, uuid_str(&script->uuid), groups) == -1) {
      LOG(ERROR, "Error retrieving script groups for script uuid [%s]", uuid_str(&script->uuid));
    }

    lua_pushnumber(lua, index); // stack = 1 table, 2 index
    lua_newtable(lua); // stack = 1 table, 2 index, 3 table

    lua_pushstring(lua, "uuid"); // stack = 1 table, 2 index, 3 table, 4 uuid key
    lua_pushstring(lua, uuid_str(&script->uuid)); // stack = 1 table, 2 index, 3 table, 4 uuid key, 5 uuid value
    lua_rawset(lua, 3); // stack = 1 table, 2 index, 3 table

    lua_pushstring(lua, "path"); // stack = 1 table, 2 index, 3 table, 4 path key
    lua_pushstring(lua, script->filepath); // stack = 1 table, 2 index, 3 table, 4 path key, 5 path value
    lua_rawset(lua, 3); // stack = 1 table, 2 index, 3 table

    lua_pushstring(lua, "groups"); // stack = 1 table, 2 index, 3 table, 4 groups
    lua_newtable(lua); // stack = 1 table, 2 index, 3 table, 4 groups, 5 table

    script_group_t* script_group = NULL;
    it_t group_it = list_begin(groups);

    int group_index = 1;

    while ((script_group = it_get(group_it)) != NULL) {
      lua_pushnumber(lua, group_index); // 1 table, 2 index, 3 table, 4 "groups", 5 table, 6 group_index
      lua_pushstring(lua, script_group->name); // 1 table, 2 index, 3 table, 4 "groups", 5 table, 6 group_index, 7 group name
      lua_rawset(lua, 5);

      group_it = it_next(group_it);
      group_index++;
    }

    lua_rawset(lua, 3); // 1 table, 2 index, 3 table, 4 "groups", 5 table
    lua_rawset(lua, 1); // stack = 1 table

    iter = it_next(iter);
    index++;

    free_linked_list_t(groups);
  }

  free_linked_list_t(scripts);

  return 1;
}
