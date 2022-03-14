#ifndef MUD_LUA_STRUCT_H
#define MUD_LUA_STRUCT_H

/**
 * Typedefs
**/
typedef struct lua_State lua_state;
typedef struct entity entity_t;
typedef struct player player_t;

/**
 * Structs
**/
typedef enum struct_type {
  STRUCT_ENTITY,
  STRUCT_PLAYER
} struct_type_t;

/**
 * Function prototypes
**/
void lua_push_entity(lua_State* l, entity_t* entity);
void lua_push_player(lua_State* l, player_t* player);
entity_t* lua_to_entity(lua_State* l, int index);
player_t* lua_to_player(lua_State* l, int index);

#endif