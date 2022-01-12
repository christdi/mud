#ifndef MUD_DB_DB_H
#define MUD_DB_DB_H

/**
 * Forward declarations
**/
typedef struct account account_t;
typedef struct entity entity_t;
typedef struct sqlite3 sqlite3;
typedef struct linked_list linked_list_t;
typedef struct script script_t;

/**
 * Function prototypes
**/
int db_account_save(sqlite3* db, account_t* account);
int db_account_load(sqlite3* db, const char* username, account_t* account);
int db_account_exists(sqlite3* db, const char* username);

int db_command_find_by_name(sqlite3* db, const char* name, linked_list_t* results);

int db_entity_load_all(sqlite3* db, linked_list_t *entities);
int db_entity_save(sqlite3* db, entity_t* entity);

int db_script_load(sqlite3* db, const char* uuid, script_t* script);
int db_script_load_all(sqlite3* db, linked_list_t *scripts);

#endif