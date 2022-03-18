#ifndef MUD_LUA_HOOKS_H
#define MUD_LUA_HOOKS_H

/**
 * Typedefs
 **/
typedef struct lua_State lua_State;
typedef struct linked_list linked_list_t;
typedef struct player player_t;
typedef struct narrator narrator_t;
typedef struct event event_t;
typedef struct lua_event_data lua_event_data_t;
typedef struct state state_t;

/**
 * Functions
 **/
int lua_hook_on_startup(lua_State* l);

int lua_hook_on_entities_loaded(lua_State* l, linked_list_t* entities);
int lua_hook_on_commands_loaded(lua_State* l, linked_list_t* commands);
int lua_hook_on_actions_loaded(lua_State* l, linked_list_t* actions);

int lua_hook_on_player_connected(lua_State* l, player_t* player);
int lua_hook_on_player_disconnected(lua_State* l, player_t* player);
int lua_hook_on_player_input(lua_State* l, player_t* player, const char* input);

int lua_hook_on_narrate_event(lua_State* l, player_t* player, narrator_t* narrator, lua_event_data_t* event);

int lua_hook_on_state_enter(lua_State* l, player_t* player, state_t* state);
int lua_hook_on_state_exit(lua_State* l, player_t* player, state_t* state);
int lua_hook_on_state_input(lua_State* l, player_t* player, state_t* state, const char* input);
int lua_hook_on_state_output(lua_State* l, player_t* player, state_t* state, const char* output);
int lua_hook_on_state_tick(lua_State* l, player_t* player, state_t* state);
int lua_hook_on_state_event(lua_State* l, player_t* player, state_t* state, event_t* event);

#endif