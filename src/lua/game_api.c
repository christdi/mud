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

#define GAME_LIB_NAME "game"

static int lua_get_entities(lua_State* lua);
static int lua_new_entity(lua_State* lua);
static int lua_get_entity(lua_State* lua);
static int lua_save_entity(lua_State* lua);
static int lua_delete_entity(lua_State* lua);

static int lua_do_action(lua_State* lua);

static int lua_register_component(lua_State* lua);

static int lua_register_state(lua_State* lua);
static int lua_deregister_state(lua_State* lua);

static int lua_register_narrator(lua_State* lua);
static int lua_deregister_narrator(lua_State* lua);

static int lua_register_archetype(lua_State* lua);

static int lua_register_system(lua_State *lua);
static int lua_deregister_system(lua_State *lua);
static int lua_enable_system(lua_State *lua);
static int lua_disable_system(lua_State *lua);

static int lua_schedule_task(lua_State *lua);
static int lua_cancel_task(lua_State* lua);
static int lua_get_tasks(lua_State* lua);

static int lua_has_component(lua_State* lua);
static int lua_add_component(lua_State* lua);
static int lua_get_component(lua_State* lua);
static int lua_get_component_entities(lua_State* lua);
static int lua_get_archetype_entities(lua_State* lua);
static int lua_matches_archetype(lua_State* lua);
static int lua_event(lua_State* lua);
static int lua_shutdown(lua_State* lua);

static const struct luaL_Reg game_lib[] = {
  { "get_entities", lua_get_entities },
  { "new_entity", lua_new_entity },
  { "get_entity", lua_get_entity },
  { "save_entity", lua_save_entity },
  { "delete_entity", lua_delete_entity },

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
 * lua - Lua state instance
 *
 * Returns 0 on success
 **/
int lua_game_register_api(lua_State* lua) {
  lua_push_api_table(lua);
  
  lua_pushstring(lua, GAME_LIB_NAME);
  luaL_newlib(lua, game_lib);
  
  lua_rawset(lua, -3);

  return 0;
}

/**
 * Lua API method which returns all entities registered with the game
 *
 * lua - Lua state instance
 *
 * Returns 0 on success
 **/
static int lua_get_entities(lua_State* lua) {
  game_t* game = lua_get_game(lua);

  lua_newtable(lua);

  h_it_t iter = hash_table_iterator(game->entities);
  entity_t* entity = NULL;
  int count = 1;

  while ((entity = h_it_get(iter)) != NULL) {
    lua_pushnumber(lua, count);
    lua_push_entity(lua, entity);
    lua_settable(lua, -3);

    iter = h_it_next(iter);
    count++;
  }

  return 1;
}

/**
 * Lua API method that creates a new entity and returns a table representing iter.
 *
 * lua - Lua state instance
 *
 * Returns 0 on success
 **/
static int lua_new_entity(lua_State* lua) {
  game_t* game = lua_get_game(lua);

  entity_t* entity = ecs_new_entity(game);

  lua_push_entity(lua, entity);

  return 1;
}

/**
 * Lua API method that executes an action with a given entity.
 *
 * game.do_action(entity, action, data)
 *
 * lua - Lua state instance
 *
 * Returns 0 on success or calls LuaL_error on error
 **/
static int lua_do_action(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);
  luaL_checktype(lua, -2, LUA_TTABLE);
  luaL_checktype(lua, -3, LUA_TTABLE);

  lua_ref_t* ref = lua_new_lua_ref_t(lua, luaL_ref(lua, LUA_REGISTRYINDEX));

  action_t* action = lua_to_action(lua, -1);
  entity_t* entity = lua_to_entity(lua, -2);
  lua_pop(lua, 2);

  game_t* game = lua_get_game(lua);

  if (script_run_action_script(game, uuid_str(&action->script), entity, ref) == -1) {
    lua_free_lua_ref_t(ref);

    return luaL_error(lua, "Failed to execute action script");
  }

  lua_free_lua_ref_t(ref);

  return 2;
}

/**
 * Lua API method which allows an entity to be retrieved via uuid
 *
 * lua - Lua state instance
 **/
static int lua_get_entity(lua_State* lua) {
  const char* uuid = luaL_checkstring(lua, -1);
  lua_pop(lua, 1);

  game_t* game = lua_get_game(lua);
  entity_t* entity = ecs_get_entity(game, uuid);

  if (entity == NULL) {
    return luaL_error(lua, "No entity found for UUID [%s]", uuid);
  }

  lua_push_entity(lua, entity);

  return 1;
}

/**
 * Lua API method to persist an entity to the database.
 *
 * lua - Lua state instance
**/
static int lua_save_entity(lua_State* lua) {
  assert(lua);

  luaL_checktype(lua, -1, LUA_TTABLE);
  entity_t* entity = lua_to_entity(lua, -1);
  lua_pop(lua, 1);

  game_t* game = lua_get_game(lua);

  if (ecs_save_entity(game, entity) == -1) {
    return luaL_error(lua, "Failed to save entity");
  }
  
  return 0;
}

/**
 * Lua API method to delete an entity in the database.
 *
 * lua - Lua state instance
**/
static int lua_delete_entity(lua_State* lua) {
  assert(lua);

  luaL_checktype(lua, -1, LUA_TTABLE);
  entity_t* entity = lua_to_entity(lua, -1);
  lua_pop(lua, 1);

  game_t* game = lua_get_game(lua);

  ecs_remove_entity_from_all_components(game->components, game->archetypes, entity);

  if (ecs_delete_entity(game, entity) == -1) {
    return luaL_error(lua, "Failed to delete entity");
  }

  return 0;
}

/**
 * Lua API method to register a component with the game engine.
 * 
 * lua - Lua state instance
 * 
 * Returns 0 on success or calls LuaL_error on error
 **/
static int lua_register_component(lua_State* lua) {
  game_t* game = lua_get_game(lua);

  component_t* component = ecs_create_component_t();

  if (list_add(game->components, component) != 0) {
    return luaL_error(lua, "Unable to add component to entity");
  }

  lua_pushlightuserdata(lua, component);

  return 1;
}

/**
 * Creates a new lua_ref_t userdata with a reference to a Lua state module.
 *
 * Parameters
 *   lua - The Lua state being called from
 *
 * Returns 0 on success or luaL_error on error.
 **/
static int lua_register_state(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);

  lua_init_lua_ref_t(lua_newuserdata(lua, sizeof(lua_ref_t)), lua, luaL_ref(lua, LUA_REGISTRYINDEX));

  return 1;
}

/**
 * Lua API method that deallocates a lua_ref_t userdata referencing a Lua state module.
 *
 * lua - the lua state that called the API method
 *
 * Returns 0r  on success or luaL_error on error.
**/
static int lua_deregister_state(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TUSERDATA);

  lua_ref_t* ref = lua_touserdata(lua, -1);

  lua_release_lua_ref_t(ref);

  return 0;
}

/**
 * Creates a new lua_ref_t userdata with a reference to a Lua narration module.
 *
 * Parameters
 *   lua - The Lua state being called from
 *
 * Returns 0 on success or luaL_error on error.
 **/
static int lua_register_narrator(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);

  lua_init_lua_ref_t(lua_newuserdata(lua, sizeof(lua_ref_t)), lua, luaL_ref(lua, LUA_REGISTRYINDEX));

  return 1;
}

/**
 * API method that takes a lua_ref_t userdata and deallocates the reference.
 *
 * lua - Lua state that method was called from
 *
 * Returns 0 on success or calls luaL_error on error.
**/
static int lua_deregister_narrator(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TUSERDATA);

  lua_ref_t* ref = lua_touserdata(lua, -1);

  lua_release_lua_ref_t(ref);

  return 0;
}

/**
 * API method that adds a new archetype to the engine.
 *
 * lua - Lua state instance
 *
 * Returns 0 on success or luaL_error on error
 **/
static int lua_register_archetype(lua_State* lua) {
  int top = 0 - lua_gettop(lua);
  int index = -1;

  archetype_t* archetype = ecs_new_archetype_t();

  while (index >= top) {
    luaL_checktype(lua, index, LUA_TLIGHTUSERDATA);
    component_t* component = lua_touserdata(lua, index);
    ecs_add_archetype_component(archetype, component);

    index--;
  }

  lua_settop(lua, 0);

  game_t* game = lua_get_game(lua);
  list_add(game->archetypes, archetype);

  lua_pushlightuserdata(lua, archetype);

  return 1;
}

/**
 * API method that adds a new system to the engine.
 *
 * lua - Lua state instance
 *
 * game.register_system("name", table)
 *
 * Returns 0 on success or luaL on error
**/
static int lua_register_system(lua_State *lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);
  luaL_checktype(lua, -2, LUA_TSTRING);

  lua_ref_t* ref = lua_new_lua_ref_t(lua, luaL_ref(lua, LUA_REGISTRYINDEX));
  const char* name = lua_tostring(lua, -1);

  lua_pop(lua, 1);

  system_t* system = ecs_new_system_t(name, ref);

  game_t* game = lua_get_game(lua);

  list_add(game->systems, system);

  lua_push_system(lua, system);

  return 1;
}

/**
 * API method that removes a system from the game.
 *
 * lua - Lua state instance
 *
 * Returns 0 on success or luaL on error
**/
static int lua_deregister_system(lua_State *lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);
  system_t* system = lua_to_system(lua, -1);

  lua_pop(lua, 1);

  game_t* game = lua_get_game(lua);

  list_remove(game->systems, system);

  return 1;
}

/**
 * API method which enables a system
 *
 * lua - Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_enable_system(lua_State *lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);
  system_t* system = lua_to_system(lua, -1);
  lua_pop(lua, 1);

  ecs_enable_system(system);
  lua_push_system(lua, system);

  return 1;
}

/**
 * API method which disables a system
 *
 * lua - Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_disable_system(lua_State *lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);
  system_t* system = lua_to_system(lua, -1);
  lua_pop(lua, 1);

  ecs_disable_system(system);
  lua_push_system(lua, system);

  return 1;
}

/**
 * API method which schedules a task for execution
 *
 * lua - Lua state instance
 *
 * game.schedule_task("name", 60, task_func)
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_schedule_task(lua_State *lua) {
  luaL_checktype(lua, -1, LUA_TFUNCTION);
  luaL_checktype(lua, -2, LUA_TNUMBER);
  luaL_checktype(lua, -3, LUA_TSTRING);

  lua_ref_t* ref = lua_new_lua_ref_t(lua, luaL_ref(lua, LUA_REGISTRYINDEX));

  int execute_in = lua_tonumber(lua, -1);
  const char* name = lua_tostring(lua, -2);

  lua_pop(lua, 2);

  game_t* game = lua_get_game(lua);
  task_t* task = task_new_task_t(game, name, execute_in, ref);

  task_schedule_task(game, task);

  lua_push_task(lua, task);

  return 1;
}

/**
 * API method which cancels a task pending execution
 *
 * lua - Lua state instance
 *
 * game.cancel_task(task)
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_cancel_task(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);

  task_t* task = lua_to_task(lua, -1);
  lua_pop(lua, 1);

  game_t* game = lua_get_game(lua);

  task_cancel_task(game, task);

  return 0;
}

/**
 * API method which retrieves pending tasks
 *
 * lua - Lua state instance
 *
 * game.get_tasks()
 *
 * Returns 0 on success or calls luaL_error on error
**/
static int lua_get_tasks(lua_State* lua) {
  game_t* game = lua_get_game(lua);

  it_t iter = list_begin(game->tasks);
  task_t* task = NULL;

  int count = 1;

  lua_newtable(lua);

  while ((task = it_get(iter) ) != NULL) {
    lua_pushnumber(lua, count);
    lua_push_task(lua, task);
    lua_settable(lua, -3);

    iter = it_next(iter);
  }

  return 1;
}

/**
 * API method that returns if an entity has a given component.
 *
 * lua - The Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
 **/
static int lua_has_component(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(lua, -2, LUA_TTABLE);

  component_t* component = lua_touserdata(lua, -1);
  entity_t* entity = lua_to_entity(lua, -2);

  lua_pop(lua, 2);

  lua_pushboolean(lua, hash_table_has(component->entities, entity->id.raw));

  return 1;
}

/**
 * API method that adds a component to a given entity
 *
 * lua - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
 **/
static int lua_add_component(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TTABLE);
  luaL_checktype(lua, -2, LUA_TLIGHTUSERDATA);
  luaL_checktype(lua, -3, LUA_TTABLE);

  entity_t* entity = lua_to_entity(lua, -3);
  component_t* component = lua_touserdata(lua, -2);

  lua_ref_t* ref = lua_new_lua_ref_t(lua, luaL_ref(lua, LUA_REGISTRYINDEX));

  lua_pop(lua, 2);

  component_data_t* component_data = ecs_create_component_data_t();
  component_data->entity = entity;
  component_data->ref = ref;

  game_t* game = lua_get_game(lua);

  ecs_add_entity_to_component(component, component_data, game->archetypes, entity);

  return 0;
}

/**
 * API method that retrieves a component for a given entity
 *
 * lua - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error
 **/
static int lua_get_component(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(lua, -2, LUA_TTABLE);

  component_t* component = lua_touserdata(lua, -1);
  entity_t* entity = lua_to_entity(lua, -2);

  lua_pop(lua, 2);

  component_data_t* component_data = hash_table_get(component->entities, entity->id.raw);

  if (component_data == NULL) {
    lua_pushnil(lua);
  } else {
    lua_rawgeti(lua, LUA_REGISTRYINDEX, component_data->ref->ref);
  }

  return 1;
}

/**
 * API method that returns the entities associated with a component
 *
 * lua - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error.
 **/
static int lua_get_component_entities(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  component_t* component = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  h_it_t iter = hash_table_iterator(component->entities);
  component_data_t* component_data = NULL;

  lua_newtable(lua);
  int count = 1;

  while ((component_data = h_it_get(iter)) != NULL) {
    lua_pushnumber(lua, count);
    lua_push_entity(lua, component_data->entity);
    lua_rawset(lua, -3);

    iter = h_it_next(iter);
    count++;
  }

  return 1;
}

/**
 * API method that returns the entities associated with an archetype
 *
 * lua - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error.
 **/
static int lua_get_archetype_entities(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  archetype_t* archetype = lua_touserdata(lua, -1);
  lua_pop(lua, 1);

  h_it_t iter = hash_table_iterator(archetype->entities);
  entity_t* entity = NULL;

  lua_newtable(lua);
  int count = 1;

  while ((entity = h_it_get(iter)) != NULL) {
    lua_pushnumber(lua, count);
    lua_push_entity(lua, entity);
    lua_rawset(lua, -3);

    iter = h_it_next(iter);
    count++;
  }

  return 1;
}

/**
 * API method that returns if a given entity matches an archetype
 *
 * lua - the Lua state instance
 *
 * Returns 0 on success or calls luaL_error on error.*
 **/
static int lua_matches_archetype(lua_State* lua) {
  luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
  luaL_checktype(lua, -2, LUA_TTABLE);

  archetype_t* archetype = lua_touserdata(lua, -1);
  entity_t* entity = lua_to_entity(lua, -2);

  lua_pop(lua, 2);

  lua_pushboolean(lua, ecs_archetype_has_entity(archetype, entity));

  return 1;
}

/**
 * Submits a new lua_event to the event broker.
 *
 * Parameters
 *  lua - the lua state submitting the event
 *
 * Returns 0 on success or calls luaL_error on failure
 **/
static int lua_event(lua_State* lua) {
  game_t* game = lua_get_game(lua);

  event_submit_event(game->event_broker, event_new_event_t(LUA_EVENT, lua_new_lua_ref_t(lua, luaL_ref(lua, LUA_REGISTRYINDEX)), lua_deallocate_lua_ref_t));

  return 0;
}

/**
 * Lua API method to shut down the game
 *
 * lua - Lua state instance
 *
 * Returns 0 on success
 **/
static int lua_shutdown(lua_State* lua) {
  game_t* game = lua_get_game(lua);

  game->shutdown = 1;

  return 0;
}
