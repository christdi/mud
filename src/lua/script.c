#include <stdlib.h>
#include <assert.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "mud/command/command.h"
#include "mud/data/hash_table.h"
#include "mud/db/db.h"
#include "mud/game.h"
#include "mud/lua/common.h"
#include "mud/lua/script.h"
#include "mud/lua/db_api.h"
#include "mud/lua/game_api.h"
#include "mud/lua/log_api.h"
#include "mud/lua/player_api.h"
#include "mud/lua/script_api.h"
#include "mud/log.h"

/**
 * TODO(Chris I)
**/
script_t* create_script_t() {
  script_t* script = calloc(1, sizeof(*script));

  return script;
}

/**
 * TODO(Chris I)
**/
void free_script_t(script_t* script) {
  assert(script);

  if (script->filepath != NULL) {
    free(script->filepath);
  }

  if (script->state != NULL) {
    lua_close(script->state);
  }

  free(script);
}

/**
 * TODO(Chris I)
**/
void deallocate_script(void* value) {
  assert(value);

  free_script_t(value);
}

/**
 * TODO(Chris I)
**/
void script_set_permission(script_t* script, permission_t flag, int permitted) {
  if (permitted) {
    script->permission |= flag;
  } else {
    script->permission &= ~flag;
  }
}

/**
 * TODO(Chris I)
**/
int script_has_permission(script_t* script, permission_t flag) {
  return script->permission & flag;
}

/**
 * TODO(Chris I)
**/
int script_load(game_t* game, hash_table_t* scripts, const char* uuid, script_t** script_out) {
  assert(game);
  assert(scripts);
  assert(uuid);

  if (hash_table_has(scripts, uuid)) {
    script_t* script = hash_table_get(scripts, uuid);

    if (script_out != NULL) {
      *script_out = script;
    }
    
    return 0;
  }

  script_t* script = create_script_t();

  if (db_script_load(game->database, uuid, script) != 0) {
    LOG(ERROR, "Failed to load script with uuid [%s]", uuid);

    return -1;
  }

  if ((script->state = luaL_newstate()) == NULL) {
    LOG(ERROR, "Failed to initialise Lua state");
    return -1;
  }

  if ((lua_common_initialise_state(script->state, game)) == -1) {
    LOG(ERROR, "Failed to initialise Lua state");
    return -1;
  }  

  if (script_has_permission(script, ALLOW_STD_LIB)) {
    luaL_openlibs(script->state);
  }

  if (script_has_permission(script, ALLOW_GAME_API) && lua_game_register_api(script->state) == -1) {
    LOG(ERROR, "Failed to register Lua API with state");
    return -1;
  }

  if (script_has_permission(script, ALLOW_DB_API) && lua_db_register_api(script->state) == -1) {
    LOG(ERROR, "Failed to register Lua DB API with state");
    return -1;
  }

  if (script_has_permission(script, ALLOW_PLAYER_API) && lua_player_register_api(script->state) == -1) {
    LOG(ERROR, "Failed to register Lua player API with state");
    return -1;
  }

  if (script_has_permission(script, ALLOW_LOG_API) && lua_log_register_api(script->state) == -1) {
    LOG(ERROR, "Failed to register Lua log API with state");
    return -1;
  }

  if (script_has_permission(script, ALLOW_SCRIPT_API) && lua_script_register_api(script->state) == -1) {
    LOG(ERROR, "Failed to register Lua script API with state");
    return -1;
  }


  if (luaL_dofile(script->state, script->filepath) != 0) {
    printf("Error while loading Lua game script [%s].\n\r", lua_tostring(script->state, -1));

    return -1;
  }

  hash_table_insert(scripts, uuid_str(&script->uuid), script);

  if (script_out != NULL) {
    *script_out = script;
  }
  
  return 0;
}

/**
 * TODO(Chris I)
**/
int script_unload(hash_table_t* scripts, const char* uuid) {
  assert(scripts);
  assert(uuid);

  if (hash_table_has(scripts, uuid) != 1) {
    LOG(WARN, "Request to unload script uuid [%s] which was not loaded", uuid);

    return -1;
  }

  hash_table_delete(scripts, uuid);

  return 0;
}

/**
 * TODO(Chris I))
**/
int script_call_command(script_t* script, command_t* command, player_t* player, const char* arguments) {
  assert(script);
  assert(command);
  assert(player);
  assert(arguments);

  if (command->function == NULL) {
    LOG(ERROR, "No function specified for command [%s] with script [%s]", command->name, script->filepath);

    return -1;
  }

  if (lua_getglobal(script->state, command->function) != LUA_TFUNCTION) {
    LOG(ERROR, "Function [%s] specified for command [%s] with script [%s] was not defined", command->function, command->name, script->filepath);

    return -1;
  }

  lua_pushlightuserdata(script->state, player);
  lua_pushstring(script->state, arguments);

  if (lua_pcall(script->state, 2, 0, 0) != 0) {
    LOG(ERROR, "Error when calling function [%s] for script [%s]", lua_tostring(script->state, -1), script->filepath);

    return -1;
  }  

  return 0;
}
