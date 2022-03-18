#ifndef MUD_DB_DB_H
#define MUD_DB_DB_H

/**
 * Forward declarations
 **/
typedef struct entity entity_t;
typedef struct player player_t;
typedef struct sqlite3 sqlite3;
typedef struct linked_list linked_list_t;
typedef struct script script_t;
typedef struct state state_t;
typedef struct action action_t;

/**
 * Function prototypes
 **/
int db_command_find_all(sqlite3* db, linked_list_t* results);

int db_action_find_all(sqlite3* db, linked_list_t* results);

int db_entity_load_all(sqlite3* db, linked_list_t* entities);
int db_entity_save(sqlite3* db, entity_t* entity);
int db_entity_get_ids_by_user(sqlite3* db, const char* uuid, linked_list_t* results);

int db_script_load(sqlite3* db, const char* uuid, script_t* script);
int db_script_load_all(sqlite3* db, linked_list_t* scripts);

int db_script_sandbox_permission_by_script_id(sqlite3* db, const char* uuid, linked_list_t* results);
int db_script_script_group_by_script_id(sqlite3* db, const char* uuid, linked_list_t* results);

int db_user_authenticate(sqlite3* db, const char* username, const char* password_hash);
int db_user_load_by_username(sqlite3* db, const char* username, player_t* player);

#endif