#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "mud/data/linked_list/linked_list.h"

/**
 * Definitions
**/
#define HASH_TABLE_SIZE 16
#define HASH_BASE_VALUE 5381
#define FIVE_BITS 5u

/**
 * Structs
**/
typedef struct hash_table {
  linked_list_t * nodes[HASH_TABLE_SIZE];
} hash_table_t;

/**
 * Function prototypes
**/
hash_table_t* create_hash_table_t();
void free_hash_table_t(hash_table_t* hash_table);

int hash_table_insert(hash_table_t* table, char* key, void* value);
int hash_table_has(hash_table_t* table, char* key);
void* hash_table_get(hash_table_t* table, char* key);
void* hash_table_delete(hash_table_t* table, char* key);

#endif