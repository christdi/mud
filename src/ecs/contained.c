#include <assert.h>
#include <stdlib.h>

#include "mud/ecs/contained.h"
#include "mud/ecs/components.h"
#include "mud/data/hash_table.h"
#include "mud/log.h"


/**
 * Allocates and initialises a new contained_t struct.
 *
 * Returns a pointer to the newly allocated contained_t;
**/
contained_t * create_contained_t() {
	contained_t * contained = calloc(1, sizeof * contained);

	return contained;
}


/**
 * Frees an allocated contained_t struct.
**/
void free_contained_t(contained_t * contained) {
	assert(contained);

	free(contained);
	contained = NULL;
}


/**
 * Checks if a given entity has a contained component registered with game components..
 *
 * Accepts the following parameters;
 *   components - a pointer to a components struct containing component registration
 *   entity - a pointer to an entity struct representing the entity being checked
 *
 * Returns 1 if a contained component is found for the entity, 0 if not.
**/
int has_contained(components_t * components, entity_t * entity) {
	assert(components);
	assert(entity);

	return hash_table_has(components->contained, entity->uuid);
}


/**
 * Registers a contained with the game components.
 *
 * Accepts the following parameters:
 *   components - a pointer to a components struct representative of the game components
 *   contained - a pointer to a contained struct to be registered with game components
**/
void register_contained(components_t * components, contained_t * contained) {
	assert(components);
	assert(contained);

	if (hash_table_insert(components->contained, contained->uuid, contained) != 0 ) {
		zlog_error(gc, "Failed to register contained component for entity uuid [%s]", contained->uuid);
	}
}


/**
 * Unregisters a contained with the game components.
 *
 * Accepts the following parameters:
 *  components - a pointer to a components struct representative of the game components
 *  entity - a pointer to an entity struct representing the entity whose contained
 *    component should be unregistered.
 *
 * Returns a pointer to the contained that was unregistered if present, NULL otherwise.
**/
contained_t * unregister_contained(components_t * components, entity_t * entity) {
	assert(components);
	assert(entity);

	return (contained_t *) hash_table_delete(components->contained, entity->uuid);	
}


/**
 * Retrieves a contained component from the game components.
 *
 * Accepts the following parameters:
 *  components - a pointer to a components struct representing the game components.
 *  entity - a pointer to an entity struct representing entity whose contained component 
 *    is desired
**/
contained_t * get_contained(components_t * components, entity_t * entity) {
	assert(components);
	assert(entity);

	return (contained_t *) hash_table_get(components->contained, entity->uuid);
}


/**
 * Prints a description of a contained component into the buffer pointed to by dest.
 *
 * Accepts the following parameters:
 *   contained - a pointer to the contained struct to be described
 *   dest - the destination string buffer for the description to be stored.  This
 *     must be allocated by the caller.
 *   len - The size of the destination string buffer.
**/
void describe_contained(contained_t * contained, char * dest, size_t len) {
	snprintf(dest, len, "container = %s", contained->container);
}

/**
 * Update all contained components.
 *
 * ccepts the following parameters:
 *  game - A pointer to a game struct containing the components structure to be updated
**/
void update_contained(game_t * game) {
}