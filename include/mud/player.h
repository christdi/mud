#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "mud/util/mudhash.h"
#include "mud/util/muduuid.h"

/**
 * Defines
 **/
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 30
#define SEND_SIZE 1024
#define COMMAND_SIZE 256
#define TOPIC_LEN 128
#define MSG_LEN 1024
/**
 * Typedefs
 **/
typedef struct client client_t;
typedef struct game game_t;
typedef struct entity entity_t;
typedef struct event event_t;
typedef struct linked_list linked_list_t;
typedef struct lua_ref lua_ref_t;
typedef struct command_repository command_repository_t;
typedef struct command_group command_group_t;

/**
 * Structs
 **/
typedef struct player {
  mud_uuid_t uuid;
  mud_uuid_t user_uuid;

  char* username;

  client_t* client;
  entity_t* entity;
  lua_ref_t* state;
  lua_ref_t* narrator;

  command_repository_t* commands;
} player_t;

/**
 * Function prototypes
 **/
player_t* create_player_t();
void free_player_t(player_t* player);
void deallocate_player(void* value);

void player_connected(client_t* client, void* context);
void player_disconnected(client_t* client, void* context);
void player_input(client_t* client, void* context);
void player_output(client_t* client, void* context);
void player_gmcp(client_t* client, void* context, const char* topic, const char* message);
void player_on_event(player_t* player, game_t* game, event_t* event);

int player_change_state(player_t* player, game_t* game, lua_ref_t* state);
int player_authenticate(player_t* player, game_t* game, const char* username, const char* password);
int player_narrate(player_t* player, game_t* game, event_t* event);
int player_request_disable_echo(player_t* player);
int player_request_enable_echo(player_t* player);
int player_add_command_group(player_t* player, command_group_t* group);
int player_remove_command_group(player_t* player, command_group_t* group);

void send_to_player(player_t* player, const char* fmt, ...);
void send_gmcp_to_player(player_t* player, char* topic, char* msg);
void send_to_players(linked_list_t* players, const char* fmt, ...);
void send_to_all_players(game_t* game, player_t* excluding, const char* fmt, ...);

#endif