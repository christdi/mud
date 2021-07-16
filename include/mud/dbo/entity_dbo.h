#ifndef MUD_DBO_ENITTY_DBO_H
#define MUD_DBO_ENTITY_DBO_H

/**
 * Typedefs
**/
typedef struct game game_t;
typedef struct entity entity_t;
typedef struct linked_list linked_list_t;

/**
 * Structs
**/
typedef struct entity_dbo {
  char* uuid;
  char* name;
  char* description;
} entity_dbo_t;

/**
 * Function prototypes
**/
entity_dbo_t* entity_dbo_t_new();
void entity_dbo_t_free(entity_dbo_t* entity_dbo);
void entity_dbo_t_deallocate(void* value);
int entity_dbo_save(game_t* game, entity_dbo_t* entity_dbo);
int entity_dbo_load_all(game_t* game, linked_list_t* entities);
void entity_dbo_from_entity(entity_dbo_t* entity_dbo, entity_t* entity);

#endif