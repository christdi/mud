#ifndef MUD_LUA_STRUCT_H
#define MUD_LUA_STRUCT_H

/**
 * Typedefs
 **/
typedef struct lua_State lua_state;
typedef struct entity entity_t;
typedef struct player player_t;
typedef struct command command_t;
typedef struct action action_t;

/**
 * Structs
 **/
typedef enum struct_type {
  STRUCT_ENTITY,
  STRUCT_PLAYER,
  STRUCT_COMMAND,
  STRUCT_ACTION
} struct_type_t;

/**
 * Function prototypes
 **/
void lua_push_entity(lua_State* l, entity_t* entity);
void lua_push_player(lua_State* l, player_t* player);
void lua_push_command(lua_State* l, command_t* command);
void lua_push_action(lua_State* l, action_t* action);

entity_t* lua_to_entity(lua_State* l, int index);
player_t* lua_to_player(lua_State* l, int index);
command_t* lua_to_command(lua_State* l, int index);
action_t* lua_to_action(lua_State* l, int index);

#endif