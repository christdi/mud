#ifndef MUD_LUA_STRUCT_H
#define MUD_LUA_STRUCT_H

/**
 * Typedefs
 **/
typedef struct lua_State lua_state;
typedef struct entity entity_t;
typedef struct player player_t;
typedef struct command command_t;
typedef struct command_group command_group_t;
typedef struct action action_t;
typedef struct system system_t;
typedef struct task task_t;
typedef struct json_node json_node_t;

/**
 * Structs
 **/
typedef enum struct_type {
  STRUCT_ENTITY,
  STRUCT_PLAYER,
  STRUCT_COMMAND,
  STRUCT_COMMAND_GROUP,
  STRUCT_ACTION,
  STRUCT_SYSTEM,
  STRUCT_TASK,
  STRUCT_JSON_NODE
} struct_type_t;

/**
 * Function prototypes
 **/
void lua_push_entity(lua_State* l, entity_t* entity);
void lua_push_player(lua_State* l, player_t* player);
void lua_push_command(lua_State* l, command_t* command);
void lua_push_command_group(lua_State* l, command_group_t* group);
void lua_push_action(lua_State* l, action_t* action);
void lua_push_system(lua_State* l, system_t* system);
void lua_push_task(lua_State* l, task_t* task);
void lua_push_json_node(lua_State* l, json_node_t* node);

entity_t* lua_to_entity(lua_State* l, int index);
player_t* lua_to_player(lua_State* l, int index);
command_t* lua_to_command(lua_State* l, int index);
command_group_t* lua_to_command_group(lua_State* l, int index);
action_t* lua_to_action(lua_State* l, int index);
system_t* lua_to_system(lua_State* l, int index);
task_t* lua_to_task(lua_State* l, int index);
json_node_t* lua_to_json_node(lua_State* l, int index);

#endif