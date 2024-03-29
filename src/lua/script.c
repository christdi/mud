#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "mud/data/linked_list.h"
#include "mud/db.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/ref.h"
#include "mud/lua/script.h"
#include "mud/lua/struct.h"
#include "mud/util/muduuid.h"

static int build_environment_table(game_t* game, const char* script_uuid);

/**
 * Allocates a new instance of script_group_t.
 *
 * name - name of the script group
 * filepath - file path to the permissions script
 * name - name of the script group
 * description - description of the script group
 *
 * Returns the allocated instance of script_group_t.
 **/
script_group_t* script_new_script_group_t(const char* uuid, const char*filepath, const char* name, const char* description) {
  assert(name);

  script_group_t* script_group = calloc(1, sizeof(script_group_t));

  script_group->uuid = str_uuid(uuid);
  script_group->filepath = strdup(filepath);
  script_group->name = strdup(name);
  script_group->description = strdup(description);

  return script_group;
}

/**
 * Frees an allocated instance of script_group_t.
 *
 * script_group - script_group_t instance to be freed.
 **/
void script_free_script_group_t(script_group_t* script_group) {
  assert(script_group);

  if (script_group->filepath != NULL) {
    free(script_group->filepath);
  }

  if (script_group->name != NULL) {
    free(script_group->name);
  }

  if (script_group->description != NULL) {
    free(script_group->description);
  }

  free(script_group);
}

/**
 * Deallocates a void pointer to script_group_t.
 *
 * value - void pointer to script_group_t
 **/
void script_deallocate_script_group_t(void* value) {
  assert(value);

  script_free_script_group_t(value);
}

/**
 * Allocates a new instance of script_t.
 *
 * Returns the newly allocated script_t.
 **/
script_t* create_script_t() {
  script_t* script = calloc(1, sizeof(*script));

  return script;
}

/**
 * Frees an allocated instance of script_t.
 *
 * Paramters
 *   script - The script_t instance to be freed.
 **/
void free_script_t(script_t* script) {
  assert(script);

  if (script->filepath != NULL) {
    free(script->filepath);
  }

  free(script);
}

/**
 * Deallocates a script_t stored in a void*
 *
 * Paramters
 *   value - void* pointing towards a script_t
 **/
void deallocate_script(void* value) {
  assert(value);

  free_script_t(value);
}

/**
 * Runs a command script.  The file is loaded and compiled and then a sandbox environment table is built
 * based on the access level of the script limiting the methods it may call in Lua.  Player and arguments
 * are exposed to the script via the environment table as "p" and "arg".
 *
 * uuid - UUID of the script to run
 * player - Player running the command
 * argument - Argument sprovided for the command
 *
 * Returns 0 on success or -1 on failure.
 **/
int script_run_command_script(game_t* game, const char* uuid, player_t* player, const char* arguments) {
  assert(game);
  assert(uuid);
  assert(player);
  assert(arguments);

  script_t* script = create_script_t();

  if (db_script_load(game->database, uuid, script) != 0) {
    LOG(ERROR, "Failed to load script with uuid [%s]", uuid);
    free_script_t(script);

    return -1;
  }

  if (luaL_loadfile(game->lua_state, script->filepath) != 0) {
    LOG(ERROR, "Error while loading Lua game script [%s].\n\r", lua_tostring(game->lua_state, -1));
    free_script_t(script);

    return -1;
  }  

  if (build_environment_table(game, uuid_str(&script->uuid)) != 0) {
    LOG(ERROR, "Error building script white list environment");
    free_script_t(script);

    return -1;
  }

  lua_pushstring(game->lua_state, "p");
  lua_push_player(game->lua_state, player);
  lua_settable(game->lua_state, -3);

  lua_pushstring(game->lua_state, "arg");
  lua_pushstring(game->lua_state, arguments);
  lua_settable(game->lua_state, -3);

  lua_setupvalue(game->lua_state, 1, 1);

  if (lua_pcall(game->lua_state, 0, 0, 0) != 0) {
    LOG(ERROR, "Error when calling command script [%s]", lua_tostring(game->lua_state, -1));

    return -1;
  }

  free_script_t(script);

  return 0;
}

/**
 * Runs a script which defines the steps of an action.
 *
 * game - game_t instance containing core game data
 * uuid - uuid of the script to run
 * entity - entity who is performing this action
 * ref - A lua ref to a table containing data relevant to action
 *
 * Returns 0 on success or -1 on failure
 **/
int script_run_action_script(game_t* game, const char* uuid, entity_t* entity, lua_ref_t* ref) {
  assert(game);
  assert(uuid);
  assert(entity);

  script_t* script = create_script_t();

  if (db_script_load(game->database, uuid, script) != 0) {
    LOG(ERROR, "Failed to load script with uuid [%s]", uuid);
    free_script_t(script);

    return -1;
  }

  if (luaL_loadfile(game->lua_state, script->filepath) != 0) {
    LOG(ERROR, "Error while loading Lua game script [%s].\n\r", lua_tostring(game->lua_state, -1));
    free_script_t(script);

    return -1;
  }

  if (build_environment_table(game, uuid_str(&script->uuid)) != 0) {
    LOG(ERROR, "Error building script white list environment");
    free_script_t(script);

    return -1;
  }

  lua_pushstring(game->lua_state, "entity");
  lua_push_entity(game->lua_state, entity);
  lua_settable(game->lua_state, -3);

  lua_pushstring(game->lua_state, "data");
  lua_rawgeti(game->lua_state, LUA_REGISTRYINDEX, ref->ref);
  lua_settable(game->lua_state, -3);

  lua_setupvalue(game->lua_state, 1, 1);

  if (lua_pcall(game->lua_state, 0, 2, 0) != 0) {
    LOG(ERROR, "Error when calling action script [%s]", lua_tostring(game->lua_state, -1));

    return -1;
  }

  free_script_t(script);

  return 0;
}

/**
 * Constructs the _ENV table to be used by a user script with appropriate permissions
 *
 * game - Game struct containing Lua state and database
 * script_uuid - UUID of scritt
 *
 * Returns 0 on success or -1 on failure
 **/
static int build_environment_table(game_t* game, const char* script_uuid) {
  linked_list_t* groups = create_linked_list_t();
  groups->deallocator = script_deallocate_script_group_t;

  if (db_script_sandbox_group_by_script_id(game->database, script_uuid, groups) < 0) {
    LOG(ERROR, "Error retrieving script groups");

    free_linked_list_t(groups);

    return -1;
  }

  lua_newtable(game->lua_state);

  it_t it = list_begin(groups);
  script_group_t* group = NULL;

  while ((group = it_get(it)) != NULL) {
    if (luaL_dofile(game->lua_state, group->filepath) != 0) {
      LOG(ERROR, "Error while running Lua sandbox script [%s]", lua_tostring(game->lua_state, -1));

      it = it_next(it);

      continue;
    }

    if (lua_type(game->lua_state, -1) != LUA_TTABLE) {
      LOG(ERROR, "Script group script [%s] didn't return a table", group->filepath);

      it = it_next(it);

      continue;
    }

    lua_pushnil(game->lua_state);

    while(lua_next(game->lua_state, -2) != 0) {
      const char* key = lua_tostring(game->lua_state, -2);
      lua_rawset(game->lua_state, -4); 
      lua_pushstring(game->lua_state, key);
    }

    lua_pop(game->lua_state, 1);

    it = it_next(it);
  }

  free_linked_list_t(groups);

  return 0;
}
