#include "lauxlib.h"
#include "lua.h"

#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/component.h"
#include "mud/ecs/entity.h"
#include "mud/event/event.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/common.h"
#include "mud/lua/event.h"
#include "mud/lua/game_api.h"
#include "mud/narrator.h"
#include "mud/state/state.h"

#define API_TABLE_NAME "game"

static int lua_new_entity(lua_State* l);
static int lua_get_entity(lua_State* l);
static int lua_get_entity_id(lua_State* l);
static int lua_register_component(lua_State* l);
static int lua_register_state(lua_State* l);
static int lua_register_narrator(lua_State* l);
static int lua_has_component(lua_State* l);
static int lua_add_component(lua_State* l);
static int lua_get_component(lua_State* l);
static int lua_event(lua_State* l);
static int lua_shutdown(lua_State* l);

static const struct luaL_Reg game_lib[] = {
  { "new_entity", lua_new_entity },
  { "get_entity", lua_get_entity },
  { "get_entity_id", lua_get_entity_id },
  { "register_component", lua_register_component },
  { "register_state", lua_register_state },
  { "register_narrator", lua_register_narrator },
  { "has_component", lua_has_component },
  { "add_component", lua_add_component },
  { "get_component", lua_get_component },
  { "event", lua_event },
  { "shutdown", lua_shutdown },
  { NULL, NULL }
};

/**
 * TODO(Chris I)
**/
int lua_game_register_api(lua_State* l) {
  luaL_newlib(l, game_lib);
  lua_setglobal(l, API_TABLE_NAME);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_new_entity(lua_State* l) {
  lua_common_assert_n_arguments(l, 2);

  const char* name = luaL_checkstring(l, 1);
  const char* description = luaL_checkstring(l, 2);

  game_t* game = lua_common_get_game(l);

  entity_t* entity = new_entity(game, name, description);

  lua_settop(l, 0);

  lua_newtable(l);
  lua_pushstring(l, "uuid");
  lua_pushstring(l, uuid_str(&entity->id));
  lua_rawset(l, -3);

  lua_pushstring(l, "name");
  lua_pushstring(l, entity->name);
  lua_rawset(l, -3);

  lua_pushstring(l, "description");
  lua_pushstring(l, entity->description);
  lua_rawset(l, -3);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_get_entity(lua_State* l) {
  lua_common_assert_n_arguments(l, 1);

  game_t* game = lua_common_get_game(l);

  const char* uuid = luaL_checkstring(l, 1);
  lua_pop(l, 1);

  entity_t* entity = get_entity(game, uuid);

  if (entity == NULL) {
    return luaL_error(l, "No entity found for UUID [%s]", uuid);
  }

  lua_newtable(l);
  lua_pushstring(l, "uuid");
  lua_pushstring(l, uuid);
  lua_rawset(l, -3);

  lua_pushstring(l, "name");
  lua_pushstring(l, entity->name);
  lua_rawset(l, -3);

  lua_pushstring(l, "description");
  lua_pushstring(l, entity->description);
  lua_rawset(l, -3);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_get_entity_id(lua_State* l) {
  lua_common_assert_n_arguments(l, 1);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);

  entity_t* entity = lua_touserdata(l, 1);

  lua_pushstring(l, entity->id.raw);

  lua_pop(l, 1);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_register_component(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  component_t* component = create_component_t();

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
  lua_common_assert_n_arguments(l, 1);
  luaL_checktype(l, 1, LUA_TTABLE);

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
  lua_common_assert_n_arguments(l, 1);
  luaL_checktype(l, 1, LUA_TTABLE);

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);

  narrator_t* narrator = lua_newuserdata(l, sizeof(narrator_t));
  narrator->ref = ref;

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_has_component(lua_State* l) {
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 2, LUA_TLIGHTUSERDATA);

  entity_t* entity = lua_touserdata(l, 1);
  component_t* component = lua_touserdata(l, 2);

  lua_pop(l, 2);

  lua_pushboolean(l, hash_table_has(component->entities, entity->id.raw));

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_add_component(lua_State* l) {
  lua_common_assert_n_arguments(l, 3);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 2, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 3, LUA_TTABLE);

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);

  entity_t* entity = lua_touserdata(l, 1);
  component_t* component = lua_touserdata(l, 2);

  component_data_t* component_data = create_component_data_t();
  component_data->ref = ref;

  hash_table_insert(component->entities, entity->id.raw, component_data);

  lua_pop(l, 2);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_get_component(lua_State* l) {
  lua_common_assert_n_arguments(l, 2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  luaL_checktype(l, 2, LUA_TLIGHTUSERDATA);

  entity_t* entity = lua_touserdata(l, 1);
  component_t* component = lua_touserdata(l, 2);

  component_data_t* component_data = hash_table_get(component->entities, entity->id.raw);

  if (component_data == NULL) {
    return 0;
  }

  lua_pop(l, 2);

  lua_rawgeti(l, LUA_REGISTRYINDEX, component_data->ref);

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
  lua_common_assert_n_arguments(l, 1);

  game_t* game = lua_common_get_game(l);

  lua_event_data_t* lua_event_data = lua_new_lua_event_data_t(l, luaL_ref(l, LUA_REGISTRYINDEX));

  event_submit_event(game->event_broker, event_new_event_t(LUA_EVENT, lua_event_data, lua_deallocate_lua_event_data_t));

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_shutdown(lua_State* l) {
  game_t* game = lua_common_get_game(l);

  game->shutdown = 1;

  return 0;
}
