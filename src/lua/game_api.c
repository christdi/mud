#include "lua.h"
#include "lauxlib.h"

#include "mud/game.h"
#include "mud/data/linked_list.h"
#include "mud/data/hash_table.h"
#include "mud/ecs/component.h"
#include "mud/ecs/entity.h"
#include "mud/lua/game_api.h"
#include "mud/log.h"

#define GLOBAL_GAME_FIELD_NAME "gptr"
#define API_TABLE_NAME "game"

static game_t* get_game_global(lua_State *l);

static int lua_new_entity(lua_State *l);
static int lua_get_entity(lua_State *l);
static int lua_get_entity_id(lua_State* l);
static int lua_register_component(lua_State* l);
static int lua_has_component(lua_State* l);
static int lua_add_component(lua_State* l);
static int lua_get_component(lua_State* l);

static const struct luaL_Reg mud_lib [] = {
  {"new_entity", lua_new_entity},
  {"get_entity", lua_get_entity},
  {"get_entity_id", lua_get_entity_id},
  {"register_component", lua_register_component},
  {"has_component", lua_has_component},
  {"add_component", lua_add_component},
  {"get_component", lua_get_component},  
  {NULL, NULL}
};

/**
 * TODO(Chris I)
**/
static game_t* get_game_global(lua_State *l) {
  lua_getglobal(l, GLOBAL_GAME_FIELD_NAME);

  int top = lua_gettop(l);

  if (lua_islightuserdata(l, top) != 1) {
    LOG(ERROR, "Could not retrieve game pointer");

    return NULL;
  }

  game_t* game = lua_touserdata(l, top);
  lua_pop(l, 1);

  return game;  
}

/**
 * TODO(Chris I)
**/
int lua_game_register_api(lua_State* l, game_t* game) {
  luaL_newlib(l, mud_lib);
  lua_setglobal(l, API_TABLE_NAME);

  lua_pushlightuserdata(l, game);
  lua_setglobal(l, GLOBAL_GAME_FIELD_NAME);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_new_entity(lua_State *l) {
  const char* name = luaL_checkstring(l, 1);
  const char *description = luaL_checkstring(l, 2);
  lua_settop(l, 0);

  game_t* game = NULL;
  
  if ((game = get_game_global(l)) == NULL) {
    lua_pushliteral(l, "lua_new_entity(): Could not retrieve game pointer");
    lua_error(l);
  }

  entity_t* entity = new_entity(game, name, description);

  lua_pushlightuserdata(l, entity);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_get_entity(lua_State *l) {
  game_t* game = NULL;

  if ((game = get_game_global(l)) == NULL) {
    lua_pushliteral(l, "lua_get_entity(): Could not retrieve game pointer");
    lua_error(l);
  }

  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_get_entity(): Expected one argument");
    lua_error(l);
  }

  const char* uuid = luaL_checkstring(l, 1);

  entity_t* entity = get_entity(game, uuid);

  if (entity == NULL) {
    lua_pushliteral(l, "No entity found");
    lua_error(l);

    return 0;
  }

  lua_pop(l, 1);

  lua_pushlightuserdata(l, entity);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_get_entity_id(lua_State* l) {
  if (lua_gettop(l) != 1) {
    lua_pushliteral(l, "lua_get_entity_id(): Expected one argument");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_get_entity_id(): First argument should be C userdata pointer to entity but was not");
    lua_error(l);
  }

  entity_t* entity = lua_touserdata(l, 1);

  lua_pushstring(l, entity->id.uuid);

  lua_pop(l, 1);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_register_component(lua_State* l) {
  game_t* game = NULL;
  
  if ((game = get_game_global(l)) == NULL) {
    lua_pushliteral(l, "lua_register_component(): Could not retrieve pointer to game");
    lua_error(l);

    return 0;
  }

  component_t* component = create_component_t();

  if (list_add(game->components, component) != 0) {
    lua_pushliteral(l, "lua_register_component(): Unable to add component to entity");
    lua_error(l);
  }

  lua_pushlightuserdata(l, component);

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_has_component(lua_State* l) {
  if (lua_gettop(l) != 2) {
    lua_pushliteral(l, "lua_has_component(): Expected 2 arguments");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_has_component(): First argument should be C userdata pointer to entity but was not");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 2) != 1) {
    lua_pushliteral(l, "lua_has_component(): Second argument should be C userdata pointer to component but was not");
    lua_error(l);
  }

  component_t* component = lua_touserdata(l, -1);
  lua_pop(l, 1);

  entity_t* entity = lua_touserdata(l, -1);
  lua_pop(l, 1);

  lua_pushboolean(l, hash_table_has(component->entities, entity->id.uuid));

  return 1;
}

/**
 * TODO(Chris I)
**/
static int lua_add_component(lua_State* l) {
  if (lua_gettop(l) != 3) {
    lua_pushliteral(l, "lua_add_component(): Expected 3 arguments");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_add_component(): First argument should be C userdata pointer to entity but was not");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 2) != 1) {
    lua_pushliteral(l, "lua_add_component(): Second argument should be C userdata pointer to component but was not");
    lua_error(l);
  }

  if (lua_istable(l, 3) != 1) {
    lua_pushliteral(l, "lua_add_component(): Third argument should be table but was not");
    lua_error(l);
  }

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);
  component_t* component = lua_touserdata(l, -1);
  lua_pop(l, 1);

  entity_t* entity = lua_touserdata(l, -1);
  lua_pop(l, 1);

  component_data_t* component_data = create_component_data_t();
  component_data->ref = ref;

  hash_table_insert(component->entities, entity->id.uuid, component_data);

  return 0;
}

/**
 * TODO(Chris I)
**/
static int lua_get_component(lua_State* l) {
  if (lua_gettop(l) != 2) {
    lua_pushliteral(l, "lua_get_component(): Expected 2 arguments");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 1) != 1) {
    lua_pushliteral(l, "lua_get_component(): First argument was not a light userdata");
    lua_error(l);
  }

  if (lua_islightuserdata(l, 2) != 1) {
    lua_pushliteral(l, "lua_get_component(): Second argument was not a light userdata");
    lua_error(l);    
  }

  component_t* component = lua_touserdata(l, -1);
  lua_pop(l, 1);

  entity_t* entity = lua_touserdata(l, -1);
  lua_pop(l, 1);

  component_data_t* component_data = hash_table_get(component->entities, entity->id.uuid);

  if (component_data == NULL) {
    return 0;
  }

  lua_rawgeti(l, LUA_REGISTRYINDEX, component_data->ref);

  return 1;
}

