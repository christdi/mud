#ifndef MUD_LUA_REPOSITORY_H
#define MUD_LUA_REPOSITORY_H

/**
 * Typedefs
**/
typedef struct hash_table hash_table_t;
typedef struct linked_list linked_list_t;
typedef struct game game_t;

typedef struct script_repository {
  hash_table_t* scripts;
  linked_list_t* pending_add;
  linked_list_t* pending_rem;
} script_repository_t;

/**
 * Functions
**/
script_repository_t* create_script_repository_t();
void free_script_repository_t(script_repository_t* repository);

int script_repository_delete(script_repository_t* repository, const char* uuid);
int script_repository_load(script_repository_t* repository, game_t* game, const char* uuid, script_t** script_out);
int script_repository_reload(script_repository_t* repository, game_t* game, const char* uuid, script_t** script_out);
void script_repository_update(script_repository_t* repository);

#endif