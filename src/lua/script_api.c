#include "lua.h"
#include "lauxlib.h"

#include "mud/data/hash_table.h"
#include "mud/db/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/script.h"
#include "mud/lua/script_api.h"
#include "mud/util/muduuid.h"

static int lua_script_load(lua_State *l);
static int lua_script_loaded(lua_State *l);
static int lua_script_available(lua_State *l);
static int lua_script_unload(lua_State *l);

static const struct luaL_Reg script_lib [] = {
  {"load", lua_script_load},
  {"unload", lua_script_unload},
  {"loaded", lua_script_loaded},
  {"available", lua_script_available},  
  {NULL, NULL}
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
static int lua_script_load(lua_State *l) {
  lua_common_assert_n_arguments(l, 1);

  game_t* game = lua_common_get_game(l);

  const char* uuid = luaL_checkstring(l, 1);

  script_load(game, game->scripts, uuid, NULL);

  lua_pop(l, 1);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_script_unload(lua_State *l) {
  lua_common_assert_n_arguments(l, 1);

  game_t* game = lua_common_get_game(l);

  const char* uuid = luaL_checkstring(l, 1);

  if (script_unload(game->scripts, uuid) != 0) {
    return luaL_error(l, "Failed to unload script with uuid [%s]", uuid);
  }

  lua_pop(l, 1);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_script_loaded(lua_State *l) {
  lua_common_assert_n_arguments(l, 0);

  game_t* game = lua_common_get_game(l);

  h_it_t it = hash_table_iterator(game->scripts);

  script_t* script = NULL;
  int index = 1;

  lua_newtable(l); // stack = 1 table

  while ((script = h_it_get(it)) != NULL) {
    lua_pushnumber(l, index); // stack = 1 table, 2 index
    lua_newtable(l); // stack = 1 table, 2 index, 3 table
    
    lua_pushstring(l, "uuid"); // stack = 1 table, 2 index, 3 table, 4 uuid key
    lua_pushstring(l, uuid_str(&script->uuid)); // stack = 1 table, 2 index, 3 table, 4 uuid key, 5 uuid value
    lua_rawset(l, 3); // stack = 1 table, 2 index, 3 table

    lua_pushstring(l, "path"); // stack = 1 table, 2 index, 3 table, 4 path key
    lua_pushstring(l, script->filepath); // stack = 1 table, 2 index, 3 table, 4 path key, 5 path value
    lua_rawset(l, 3); // stack = 1 table, 2 index, 3 table

    lua_rawset(l, 1); // stack = 1 table

    it = h_it_next(it);
    index++;
  }

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_script_available(lua_State *l) {
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
    lua_pushnumber(l, index); // stack = 1 table, 2 index
    lua_newtable(l); // stack = 1 table, 2 index, 3 table

    lua_pushstring(l, "uuid"); // stack = 1 table, 2 index, 3 table, 4 uuid key
    lua_pushstring(l, uuid_str(&script->uuid)); // stack = 1 table, 2 index, 3 table, 4 uuid key, 5 uuid value
    lua_rawset(l, 3); // stack = 1 table, 2 index, 3 table

    lua_pushstring(l, "path"); // stack = 1 table, 2 index, 3 table, 4 path key
    lua_pushstring(l, script->filepath); // stack = 1 table, 2 index, 3 table, 4 path key, 5 path value
    lua_rawset(l, 3); // stack = 1 table, 2 index, 3 table

    lua_rawset(l, 1); // stack = 1 table

    it = it_next(it);
    index++;
  }

  free_linked_list_t(scripts);

  return 1;
}
