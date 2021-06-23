#ifndef _ECS_ENTITY_ITEM_H_
#define _ECS_ENTITY_ITEM_H_


/**
 * Typedefs
**/
typedef struct entity entity_t;
typedef struct game game_t;


/**
 * Function prototypes
**/
entity_t * new_item(game_t * game, char * name, char * description);


#endif