#include "lauxlib.h"
#include "lua.h"

#include "mud/data/hash_table.h"
#include "mud/db/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/script.h"
#include "mud/lua/script_api.h"
#include "mud/util/muduuid.h"

static int lua_script_available(lua_State* l);

static const struct luaL_Reg script_lib[] = {
  { "available", lua_script_available },
  { NULL, NULL }
};

/**
 * TODO(Chris I)
 **/
int lua_script_register_api(lua_State* l) {
  luaL_newlib(l, script_lib);
  lua_setglobal(l, "script");

  return 0;
}

/**
 * TODO(Chris I)
 **/
static int lua_script_available(lua_State* l) {
  lua_common_assert_n_arguments(l, 0);

  game_t* game = lua_common_get_game(l);

  linked_list_t* scripts = create_linked_list_t();
  scripts->deallocator = deallocate_script;

  if (db_script_load_all(game->database, scripts) == -1) {
    LOG(ERROR, "Error loading scripts from database");

    return -1;
  }

  script_t* script = NULL;
  it_t it = list_begin(scripts);
  int index = 1;

  lua_newtable(l); // stack = 1 table

  while ((script = it_get(it)) != NULL) {
    linked_list_t* groups = create_linked_list_t();
    groups->deallocator = script_deallocate_script_group_t;

    if (db_script_script_group_by_script_id(game->database, uuid_str(&script->uuid), groups) == -1) {
      LOG(ERROR, "Error retrieving script groups for script uuid [%s]", uuid_str(&script->uuid));
    }

    lua_pushnumber(l, index); // stack = 1 table, 2 index
    lua_newtable(l); // stack = 1 table, 2 index, 3 table

    lua_pushstring(l, "uuid"); // stack = 1 table, 2 index, 3 table, 4 uuid key
    lua_pushstring(l, uuid_str(&script->uuid)); // stack = 1 table, 2 index, 3 table, 4 uuid key, 5 uuid value
    lua_rawset(l, 3); // stack = 1 table, 2 index, 3 table

    lua_pushstring(l, "path"); // stack = 1 table, 2 index, 3 table, 4 path key
    lua_pushstring(l, script->filepath); // stack = 1 table, 2 index, 3 table, 4 path key, 5 path value
    lua_rawset(l, 3); // stack = 1 table, 2 index, 3 table

    lua_pushstring(l, "groups"); // stack = 1 table, 2 index, 3 table, 4 groups
    lua_newtable(l); // stack = 1 table, 2 index, 3 table, 4 groups, 5 table

    script_group_t* script_group = NULL;
    it_t group_it = list_begin(groups);

    int group_index = 1;

    while ((script_group = it_get(group_it)) != NULL) {
      lua_pushnumber(l, group_index); // 1 table, 2 index, 3 table, 4 "groups", 5 table, 6 group_index
      lua_pushstring(l, script_group->name); // 1 table, 2 index, 3 table, 4 "groups", 5 table, 6 group_index, 7 group name
      lua_rawset(l, 5);

      group_it = it_next(group_it);
      group_index++;
    }

    lua_rawset(l, 3); // 1 table, 2 index, 3 table, 4 "groups", 5 table
    lua_rawset(l, 1); // stack = 1 table

    it = it_next(it);
    index++;

    free_linked_list_t(groups);
  }

  free_linked_list_t(scripts);

  return 1;
}
