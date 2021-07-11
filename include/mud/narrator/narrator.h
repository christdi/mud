#ifndef _NARRATOR_H_
#define _NARRATOR_H_

/**
 * Typedefs
**/
typedef struct player player_t;
typedef struct entity entity_t;
typedef struct hash_table hash_table_t;
typedef struct linked_list linked_list_t;
typedef struct entity entity_t;
typedef struct game game_t;

/**
 * Structs
**/
typedef struct narrator {
    hash_table_t* entities; 
} narrator_t;

/**
 * Function prototypes
**/
narrator_t* create_narrator_t();
void free_narrator_t(narrator_t* narrator);

int add_player_to_narration(narrator_t* narrator, entity_t* entity, player_t *player);
int remove_player_from_narration(narrator_t* narrator, entity_t* entity, player_t *player);
int retrieve_entity_listeners(narrator_t* narrator, entity_t* entity, linked_list_t** listeners);
void narrate_speak_action(game_t* game, entity_t* entity, char* what);

#endif