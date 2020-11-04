#include <assert.h>
#include <stdlib.h>

#include "mud/ecs/container.h"
#include "mud/ecs/components.h"
#include "mud/data/linked_list.h"
#include "mud/data/hash_table.h"
#include "mud/log.h"


/**
 * Allocates and initialises a new container_t struct.
 *
 * Returns a pointer to the newly allocated container_t;
**/
container_t * create_container_t() {
	container_t * container = calloc(1, sizeof * container);

	container->contains = create_linked_list_t();

	return container;
}


/**
 * Frees an allocated container_t struct.
**/
void free_container_t(container_t * container) {
	assert(container);

	if (container->contains) {
		free_linked_list_t(container->contains);
		container->contains = NULL;
	}

	free(container);
	container = NULL;
}


/**
 * Checks if a given entity has a container component registered with game components..
 *
 * Accepts the following parameters;
 *   components - a pointer to a components struct containing component registration
 *   entity - a pointer to an entity struct representing the entity being checked
 *
 * Returns 1 if a container component is found for the entity, 0 if not.
**/
int has_container(components_t * components, entity_t * entity) {
	assert(components);
	assert(entity);

	return hash_table_has(components->container, entity->uuid);
}


/**
 * Registers a container with the game components.
 *
 * Accepts the following parameters:
 *   components - a pointer to a components struct representative of the game components
 *   container - a pointer to a container struct to be registered with game components
**/
void register_container(components_t * components, container_t * container) {
	assert(components);
	assert(container);

	if (hash_table_insert(components->container, container->uuid, container) != 0 ) {
		zlog_error(gc, "Failed to register container component for entity uuid [%s]", container->uuid);
	}
}


/**
 * Unregisters a container with the game components.
 *
 * Accepts the following parameters:
 *  components - a pointer to a components struct representative of the game components
 *  entity - a pointer to an entity struct representing the entity whose container
 *    component should be unregistered.
 *
 * Returns a pointer to the container that was unregistered if present, NULL otherwise.
**/
container_t * unregister_container(components_t * components, entity_t * entity) {
	assert(components);
	assert(entity);

	return (container_t *) hash_table_delete(components->container, entity->uuid);	
}


/**
 * Retrieves a container component from the game components.
 *
 * Accepts the following parameters:
 *  components - a pointer to a components struct representing the game components.
 *  entity - a pointer to an entity struct representing entity whose container component 
 *    is desired
**/
container_t * get_container(components_t * components, entity_t * entity) {
	assert(components);
	assert(entity);

	return (container_t *) hash_table_get(components->container, entity->uuid);
}


/**
 * Update all container components.
 *
 * ccepts the following parameters:
 *  game - A pointer to a game struct containing the components structure to be updated
**/
void update_container(game_t * game) {
}