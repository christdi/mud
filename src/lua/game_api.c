#include "lauxlib.h"
#include "lua.h"

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/ecs.h"
#include "mud/event/event.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/event.h"
#include "mud/lua/game_api.h"
#include "mud/lua/struct.h"
#include "mud/narrator.h"
#include "mud/state/state.h"

#define API_TABLE_NAME "game"

static int lua_get_entities(lua_State *l);
static int lua_new_entity(lua_State* l);
static int lua_get_entity(lua_State* l);
static int lua_register_component(lua_State* l);
static int lua_register_state(lua_State* l);
static int lua_register_narrator(lua_State* l);
static int lua_register_archetype(lua_State* l);
static int lua_has_component(lua_State* l);
static int lua_add_component(lua_State* l);
static int lua_get_component(lua_State* l);
static int lua_get_component_entities(lua_State* l);
static int lua_get_archetype_entities(lua_State* l);
static int lua_event(lua_State* l);
static int lua_shutdown(lua_State* l);

static const struct luaL_Reg game_lib[] = {
  { "get_entities", lua_get_entities },
  { "new_entity", lua_new_entity },
  { "get_entity", lua_get_entity },
  { "register_component", lua_register_component },
  { "register_state", lua_register_state },
  { "register_narrator", lua_register_narrator },
  { "register_archetype", lua_register_archetype },
  { "has_component", lua_has_component },
  { "add_component", lua_add_component },
  { "get_component", lua_get_component },
  { "get_component_entities", lua_get_component_entities },
  { "get_archetype_entities", lua_get_archetype_entities },
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
  const char* description = luaL_checkstring(l, -1);
  const char* name = luaL_checkstring(l, -2);

  lua_pop(l, 2);

  game_t* game = lua_common_get_game(l);
  entity_t* entity = ecs_new_entity(game, name, description);

  lua_push_entity(l, entity);

  return 1;
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
 * Creates a new state_t userdata with a reference to a Lua state module.
 *
 * Parameters
 *   l - The Lua state being called from
 *
 * Returns 0 on success or luaL_error on error.
**/
static int lua_register_state(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);

  state_t* state = lua_newuserdata(l, sizeof(state_t));
  state->ref = ref;

  return 1;
}

/**
 * Creates a new narrator_t userdata with a reference to a Lua narration module.
 *
 * Parameters
 *   l - The Lua state being called from
 *
 * Returns 0 on success or luaL_error on error.
**/
static int lua_register_narrator(lua_State* l) {
  luaL_checktype(l, -1, LUA_TTABLE);

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);

  narrator_t* narrator = lua_newuserdata(l, sizeof(narrator_t));
  narrator->ref = ref;

  return 1;
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

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);

  lua_pop(l, 2);

  component_data_t* component_data = ecs_create_component_data_t();
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
    lua_rawgeti(l, LUA_REGISTRYINDEX, component_data->ref);
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
 * Submits a new lua_event to the event broker.
 *
 * Parameters
 *  l - the lua state submitting the event
 *
 * Returns 0 on success or calls luaL_error on failure
**/
static int lua_event(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  lua_event_data_t* lua_event_data = lua_new_lua_event_data_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  event_submit_event(game->event_broker, event_new_event_t(LUA_EVENT, lua_event_data, lua_deallocate_lua_event_data_t));

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
