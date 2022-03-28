#ifndef MUD_LUA_HOOKS_H
#define MUD_LUA_HOOKS_H

/**
 * Typedefs
 **/
typedef struct lua_State lua_State;
typedef struct linked_list linked_list_t;
typedef struct player player_t;
typedef struct event event_t;
typedef struct lua_event_data lua_event_data_t;
typedef struct system system_t;
typedef struct task task_t;
typedef struct lua_ref lua_ref_t;

/**
 * Functions
 **/
int lua_call_startup_hook(lua_State* l);
int lua_call_shutdown_hook(lua_State *l);

int lua_call_entities_loaded_hook(lua_State* l, linked_list_t* entities);
int lua_call_commands_loaded_hook(lua_State* l, linked_list_t* commands);
int lua_call_actions_loaded_hook(lua_State* l, linked_list_t* actions);

int lua_call_player_connected_hook(lua_State* l, player_t* player);
int lua_call_player_disconnected_hook(lua_State* l, player_t* player);
int lua_call_player_input_hook(lua_State* l, player_t* player, const char* input);

int lua_call_narrate_event_hook(lua_State* l, player_t* player, lua_ref_t* narrator, lua_ref_t* event);

int lua_call_state_enter_hook(lua_State* l, player_t* player, lua_ref_t* state);
int lua_call_state_exit_hook(lua_State* l, player_t* player, lua_ref_t* state);
int lua_call_state_input_hook(lua_State* l, player_t* player, lua_ref_t* state, const char* input);
int lua_call_state_output_hook(lua_State* l, player_t* player, lua_ref_t* state, const char* output);
int lua_call_state_event_hook(lua_State* l, player_t* player, lua_ref_t* state, event_t* event);
int lua_call_state_gmcp_hook(lua_State*l, player_t* player, lua_ref_t* state, const char* topic, const char* msg);

int lua_call_system_execute_hook(lua_State* l, system_t* system);
int lua_call_task_execute_hook(lua_State* l, task_t* task);

#endif