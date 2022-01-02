#ifndef MUD_DB_DB_H
#define MUD_DB_DB_H

/**
 * Forward declarations
**/
typedef struct account account_t;
typedef struct sqlite3 sqlite3;
/**
 * Function prototypes
**/
int db_account_save(sqlite3* db, account_t* account);
int db_account_load(sqlite3* db, const char* username, account_t* account);
int db_account_exists(sqlite3* db, const char* username);

#endif