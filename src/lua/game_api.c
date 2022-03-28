#include <assert.h>

#include "lauxlib.h"
#include "lua.h"

#include "mud/action.h"
#include "mud/command.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/ecs.h"
#include "mud/event.h"
#include "mud/game.h"
#include "mud/json.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/game_api.h"
#include "mud/lua/ref.h"
#include "mud/lua/script.h"
#include "mud/lua/struct.h"
#include "mud/task.h"

#define API_TABLE_NAME "game"

static int lua_test(lua_State* l);

static int lua_get_entities(lua_State* l);
static int lua_new_entity(lua_State* l);
static int lua_get_entity(lua_State* l);
static int lua_save_entity(lua_State* l);
static int lua_delete_entity(lua_State* l);

static int lua_do_command(lua_State* l);
static int lua_do_action(lua_State* l);

static int lua_register_component(lua_State* l);

static int lua_register_state(lua_State* l);
static int lua_deregister_state(lua_State* l);

static int lua_register_narrator(lua_State* l);
static int lua_deregister_narrator(lua_State* l);

static int lua_register_archetype(lua_State* l);

static int lua_register_system(lua_State *l);
static int lua_deregister_system(lua_State *l);
static int lua_enable_system(lua_State *l);
static int lua_disable_system(lua_State *l);

static int lua_schedule_task(lua_State *l);
static int lua_cancel_task(lua_State* l);
static int lua_get_tasks(lua_State* l);

static int lua_has_component(lua_State* l);
static int lua_add_component(lua_State* l);
static int lua_get_component(lua_State* l);
static int lua_get_component_entities(lua_State* l);
static int lua_get_archetype_entities(lua_State* l);
static int lua_matches_archetype(lua_State* l);
static int lua_event(lua_State* l);
static int lua_shutdown(lua_State* l);

static const struct luaL_Reg game_lib[] = {
  { "test", lua_test },

  { "get_entities", lua_get_entities },
  { "new_entity", lua_new_entity },
  { "get_entity", lua_get_entity },
  { "save_entity", lua_save_entity },
  { "delete_entity", lua_delete_entity },

  { "do_command", lua_do_command },
  { "do_action", lua_do_action },

  { "register_component", lua_register_component },

  { "register_state", lua_register_state },
  { "deregister_state", lua_deregister_state },

  { "register_narrator", lua_register_narrator },
  { "deregister_narrator", lua_deregister_narrator },

  { "register_archetype", lua_register_archetype },

  { "register_system", lua_register_system },
  { "deregister_system", lua_deregister_system },
  { "enable_system", lua_enable_system },
  { "disable_system", lua_disable_system },

  { "schedule_task", lua_schedule_task },
  { "cancel_task", lua_cancel_task },
  { "get_tasks", lua_get_tasks },

  { "has_component", lua_has_component },
  { "add_component", lua_add_component },
  { "get_component", lua_get_component },

  { "get_component_entities", lua_get_component_entities },
  { "get_archetype_entities", lua_get_archetype_entities },

  { "matches_archetype", lua_matches_archetype },

  { "event", lua_event },

  { "shutdown", lua_shutdown },
  { NULL, NULL }
};

/**
 * Register the game API with Lua
 *
 * l - Lua state instance
 *
 * Returns 0 on success
 **/
int lua_game_register_api(lua_State* l) {
  luaL_newlib(l, game_lib);
  lua_setglobal(l, API_TABLE_NAME);

  return 0;
}

/**
 * Lua API method which does nothing in particular but can be changed to test functionality.
**/
static int lua_test(lua_State* l) {
  return 0;
}

/**
 * Lua API method which returns all entities registered with the game
 *
 * l - Lua state instance
 *
 * Returns 0 on success
 **/
static int lua_get_entities(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  lua_newtable(l);

  h_it_t it = hash_table_iterator(game->entities);
  entity_t* entity = NULL;
  int count = 1;

  while ((entity = h_it_get(it)) != NULL) {
    lua_pushnumber(l, count);
    lua_push_entity(l, entity);
    lua_settable(l, -3);

    it = h_it_next(it);
    count++;
  }

  return 1;
}

/**
 * Lua API method that creates a new entity and returns a table representing it.
 *
 * l - Lua state instance
 *
 * Returns 0 on success
 **/
static int lua_new_entity(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  entity_t* entity = ecs_new_entity(game);

  lua_push_entity(l, entity);

  return 1;
}

/**
 * Lua API method that executes an action with a given entity.
 *
 * game.do_command(player, command, arguments)
 *
 * l - Lua state instance
 *
 * Returns 0 on success or calls LuaL_error on error
 **/
static int lua_do_command(lua_State* l) {
  luaL_checktype(l, -1, LUA_TSTRING);
  luaL_checktype(l, -2, LUA_TTABLE);
  luaL_checktype(l, -3, LUA_TTABLE);

  const char* arguments = lua_tostring(l, -1);
  command_t* command = lua_to_command(l, -2);
  player_t* player = lua_to_player(l, -3);

  lua_pop(l, 3);

  game_t* game = lua_common_get_game(l);

  if (script_run_command_script(game, uuid_str(&command->script), player, arguments) == -1) {
    return luaL_error(l, "Failed to execute command script");
  }

  return 0;
}

/**
 * Lua API method that executes an action with a given entity.
 *
 * game.do_action(entity, action, data)
 *
 * l - Lua state instance
 *
 * Returns 0 on success or calls LuaL_error on error
 **/
static int lua_do_action(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  luaL_checktype(l, -2, LUA_TTABLE);
  luaL_checktype(l, -3, LUA_TTABLE);

  lua_ref_t* ref = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  action_t* action = lua_to_action(l, -1);
  entity_t* entity = lua_to_entity(l, -2);
  lua_pop(l, 2);

  game_t* game = lua_common_get_game(l);

  if (script_run_action_script(game, uuid_str(&action->script), entity, ref) == -1) {
    lua_free_lua_ref_t(ref);

    return luaL_error(l, "Failed to execute action script");
  }

  lua_free_lua_ref_t(ref);

  return 2;
}

/**
 * Lua API method which allows an entity to be retrieved via uuid
 *
 * l - Lua state instance
 **/
static int lua_get_entity(lua_State* l) {
  const char* uuid = luaL_checkstring(l, -1);
  lua_pop(l, 1);

  game_t* game = lua_common_get_game(l);
  entity_t* entity = ecs_get_entity(game, uuid);

  if (entity == NULL) {
    return luaL_error(l, "No entity found for UUID [%s]", uuid);
  }

  lua_push_entity(l, entity);

  return 1;
}

/**
 * Lua API method to persist an entity to the database.
 *
 * l - Lua state instance
**/
static int lua_save_entity(lua_State* l) {
  assert(l);

  luaL_checktype(l, -1, LUA_TTABLE);
  entity_t* entity = lua_to_entity(l, -1);
  lua_pop(l, 1);

  game_t* game = lua_common_get_game(l);

  if (ecs_save_entity(game, entity) == -1) {
    return luaL_error(l, "Failed to save entity");
  }
  
  return 0;
}

/**
 * Lua API method to delete an entity in the database.
 *
 * l - Lua state instance
**/
static int lua_delete_entity(lua_State* l) {
  assert(l);

  luaL_checktype(l, -1, LUA_TTABLE);
  entity_t* entity = lua_to_entity(l, -1);
  lua_pop(l, 1);

  game_t* game = lua_common_get_game(l);

  ecs_remove_entity_from_all_components(game->components, game->archetypes, entity);

  if (ecs_delete_entity(game, entity) == -1) {
    return luaL_error(l, "Failed to delete entity");
  }

  return 0;
}

/**
 * TODO(Chris I)
 **/
static int lua_register_component(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  component_t* component = ecs_create_component_t();

  if (list_add(game->components, component) != 0) {
    return luaL_error(l, "Unable to add component to entity");
  }

  lua_pushlightuserdata(l, component);

  return 1;
}

/**
 * Creates a new lua_ref_t userdata with a reference to a Lua state module.
 *
 * Parameters
 *   l - The Lua state being called from
 *
 * Returns 0 on success or luaL_error on error.
 **/
static int lua_register_state(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);

  lua_init_lua_ref_t(lua_newuserdata(l, sizeof(lua_ref_t)), l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 1;
}

/**
 * Lua API method that deallocates a lua_ref_t userdata referencing a Lua state module.
 *
 * l - the lua state that called the API method
 *
 * Returns 0r  on success or luaL_error on error.
**/
static int lua_deregister_state(lua_State* l) {
  luaL_checktype(l, -1, LUA_TUSERDATA);

  lua_ref_t* ref = lua_touserdata(l, -1);

  lua_release_lua_ref_t(ref);

  return 0;
}

/**
 * Creates a new lua_ref_t userdata with a reference to a Lua narration module.
 *
 * Parameters
 *   l - The Lua state being called from
 *
 * Returns 0 on success or luaL_error on error.
 **/
static int lua_register_narrator(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);

  lua_init_lua_ref_t(lua_newuserdata(l, sizeof(lua_ref_t)), l, luaL_ref(l, LUA_REGISTRYINDEX));

  return 1;
}

/**
 * API method that takes a lua_ref_t userdata and deallocates the reference.
 *
 * l - Lua state that method was called from
 *
 * Returns 0 on success or calls luaL_error on error.
**/
static int lua_deregister_narrator(lua_State* l) {
  luaL_checktype(l, -1, LUA_TUSERDATA);

  lua_ref_t* ref = lua_touserdata(l, -1);

  lua_release_lua_ref_t(ref);

  return 0;
}

/**
 * API method that adds a new archetype to the engine.
 *
 * l - Lua state instance
 *
 * Returns 0 on success or luaL_error on error
 **/
static int lua_register_archetype(lua_State* l) {
  int top = 0 - lua_gettop(l);
  int index = -1;

  archetype_t* archetype = ecs_new_archetype_t();

  while (index >= top) {
    luaL_checktype(l, index, LUA_TLIGHTUSERDATA);
    component_t* component = lua_touserdata(l, index);
    ecs_add_archetype_component(archetype, component);

    index--;
  }

  lua_settop(l, 0);

  game_t* game = lua_common_get_game(l);
  list_add(game->archetypes, archetype);

  lua_pushlightuserdata(l, archetype);

  return 1;
}

/**
 * API method that adds a new system to the engine.
 *
 * l - Lua state instance
 *
 * game.register_system("name", table)
 *
 * Returns 0 on success or luaL on error
**/
static int lua_register_system(lua_State *l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  luaL_checktype(l, -2, LUA_TSTRING);

  lua_ref_t* ref = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));
  const char* name = lua_tostring(l, -1);

  lua_pop(l, 1);

  system_t* system = ecs_new_system_t(name, ref);

  game_t* game = lua_common_get_game(l);

  list_add(game->systems, system);

  lua_push_system(l, system);

  return 1;
}

/**
 * API method that removes a system from the game.
 *
 * l - Lua state instance
 *
 * Returns 0 on success or luaL on error
**/
static int lua_deregister_system(lua_State *l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  system_t* system = lua_to_system(l, -1);

  lua_pop(l, 1);

  game_t* game = lua_common_get_game(l);

  list_remove(game->systems, system);

  return 1;
}

/**
 * API method which enables a system
 *
 * l - Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_enable_system(lua_State *l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  system_t* system = lua_to_system(l, -1);
  lua_pop(l, 1);

  ecs_enable_system(system);
  lua_push_system(l, system);

  return 1;
}

/**
 * API method which disables a system
 *
 * l - Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_disable_system(lua_State *l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  system_t* system = lua_to_system(l, -1);
  lua_pop(l, 1);

  ecs_disable_system(system);
  lua_push_system(l, system);

  return 1;
}

/**
 * API method which schedules a task for execution
 *
 * l - Lua state instance
 *
 * game.schedule_task("name", 60, task_func)
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_schedule_task(lua_State *l) {
  luaL_checktype(l, -1, LUA_TFUNCTION);
  luaL_checktype(l, -2, LUA_TNUMBER);
  luaL_checktype(l, -3, LUA_TSTRING);

  lua_ref_t* ref = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  int execute_in = lua_tonumber(l, -1);
  const char* name = lua_tostring(l, -2);

  lua_pop(l, 2);

  task_t* task = task_new_task_t(name, execute_in, ref);
  game_t* game = lua_common_get_game(l);

  task_schedule_task(game->tasks, task);

  lua_push_task(l, task);

  return 1;
}

/**
 * API method which cancels a task pending execution
 *
 * l - Lua state instance
 *
 * game.cancel_task(task)
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_cancel_task(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);

  task_t* task = lua_to_task(l, -1);
  lua_pop(l, 1);

  game_t* game = lua_common_get_game(l);

  task_cancel_task(game->tasks, game, task);

  return 0;
}

/**
 * API method which retrieves pending tasks
 *
 * l - Lua state instance
 *
 * game.get_tasks()
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_get_tasks(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  it_t it = list_begin(game->tasks);
  task_t* task = NULL;

  int count = 1;

  lua_newtable(l);

  while ((task = it_get(it) ) != NULL) {
    lua_pushnumber(l, count);
    lua_push_task(l, task);
    lua_settable(l, -3);

    it = it_next(it);
  }

  return 1;
}

/**
 * API method that returns if an entity has a given component.
 *
 * l - The Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
 **/
static int lua_has_component(lua_State* l) {
  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, -2, LUA_TTABLE);

  component_t* component = lua_touserdata(l, -1);
  entity_t* entity = lua_to_entity(l, -2);

  lua_pop(l, 2);

  lua_pushboolean(l, hash_table_has(component->entities, entity->id.raw));

  return 1;
}

/**
 * API method that adds a component to a given entity
 *
 * l - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
 **/
static int lua_add_component(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);
  luaL_checktype(l, -2, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, -3, LUA_TTABLE);

  entity_t* entity = lua_to_entity(l, -3);
  component_t* component = lua_touserdata(l, -2);

  lua_ref_t* ref = lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  lua_pop(l, 2);

  component_data_t* component_data = ecs_create_component_data_t();
  component_data->entity = entity;
  component_data->ref = ref;

  game_t* game = lua_common_get_game(l);

  ecs_add_entity_to_component(component, component_data, game->archetypes, entity);

  return 0;
}

/**
 * API method that retrieves a component for a given entity
 *
 * l - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
 **/
static int lua_get_component(lua_State* l) {
  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, -2, LUA_TTABLE);

  component_t* component = lua_touserdata(l, -1);
  entity_t* entity = lua_to_entity(l, -2);

  lua_pop(l, 2);

  component_data_t* component_data = hash_table_get(component->entities, entity->id.raw);

  if (component_data == NULL) {
    lua_pushnil(l);
  } else {
    lua_rawgeti(l, LUA_REGISTRYINDEX, component_data->ref->ref);
  }

  return 1;
}

/**
 * API method that returns the entities associated with a component
 *
 * l - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error.
 **/
static int lua_get_component_entities(lua_State* l) {
  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  component_t* component = lua_touserdata(l, -1);
  lua_pop(l, 1);

  h_it_t it = hash_table_iterator(component->entities);
  component_data_t* component_data = NULL;

  lua_newtable(l);
  int count = 1;

  while ((component_data = h_it_get(it)) != NULL) {
    lua_pushnumber(l, count);
    lua_push_entity(l, component_data->entity);
    lua_rawset(l, -3);

    it = h_it_next(it);
    count++;
  }

  return 1;
}

/**
 * API method that returns the entities associated with an archetype
 *
 * l - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error.
 **/
static int lua_get_archetype_entities(lua_State* l) {
  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  archetype_t* archetype = lua_touserdata(l, -1);
  lua_pop(l, 1);

  h_it_t it = hash_table_iterator(archetype->entities);
  entity_t* entity = NULL;

  lua_newtable(l);
  int count = 1;

  while ((entity = h_it_get(it)) != NULL) {
    lua_pushnumber(l, count);
    lua_push_entity(l, entity);
    lua_rawset(l, -3);

    it = h_it_next(it);
    count++;
  }

  return 1;
}

/**
 * API method that returns if a given entity matches an archetype
 *
 * l - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error.*
 **/
static int lua_matches_archetype(lua_State* l) {
  luaL_checktype(l, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, -2, LUA_TTABLE);

  archetype_t* archetype = lua_touserdata(l, -1);
  entity_t* entity = lua_to_entity(l, -2);

  lua_pop(l, 2);

  lua_pushboolean(l, ecs_archetype_has_entity(archetype, entity));

  return 1;
}

/**
 * Submits a new lua_event to the event broker.
 *
 * Parameters
 *  l - the lua state submitting the event
 *
 * Returns 0 on success or calls luaL_error on failure
 **/
static int lua_event(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  event_submit_event(game->event_broker, event_new_event_t(LUA_EVENT, lua_new_lua_ref_t(l, luaL_ref(l, LUA_REGISTRYINDEX)), lua_deallocate_lua_ref_t));

  return 0;
}

/**
 * Lua API method to shut down the game
 *
 * l - Lua state instance
 *
 * Returns 0 on success
 **/
static int lua_shutdown(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  game->shutdown = 1;

  return 0;
}
