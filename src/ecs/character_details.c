#include "mud/ecs/character_details.h"
#include "mud/ecs/components.h"
#include "mud/data/hash_table/hash_table.h"
#include "mud/log/log.h"

#include <stdlib.h>

/**
 * Allocate memory for and initialise a new character_details_t.
 *
 * Returns the newly allocated character_details_t
**/
character_details_t * create_character_details_t() {
	character_details_t * character_details = calloc(1, sizeof * character_details);

	return character_details;
}


/**
 * Free a character_details_t.
**/
void free_character_details_t(character_details_t * character_details) {
	if (character_details->name) {
		free(character_details->name);
		character_details->name = NULL;
	}

	if (character_details->description) {
		free(character_details->description);
		character_details->description = NULL;
	}

	free(character_details);
	character_details = NULL;
}


/**
 * Determines if a given uuid character details registered.
 *
 * Returns 1 if true or 0 if false.
**/
int has_character_details(components_t * components, char * uuid) {
	return hash_table_has(components->character_details, uuid);
}

/**
 * Register a character detail components for a given uuid with active game components.
**/
void register_character_details(components_t * components, character_details_t * character_details) {
	if (hash_table_insert(components->character_details, character_details->uuid, character_details) != 0 ) {
		zlog_error(gc, "Failed to register character details for entity uuid [%s]", character_details->uuid);
	}
}

/**
 * Removes character details component for a given uuid.
 *
 * Returns the removed character_details_t in case it has to be freed, or null if
 * there was no matching uuid.
**/
character_details_t * unregister_character_details(components_t * components, char * uuid) {
	return (character_details_t *) hash_table_delete(components->character_details, uuid);
}

/**
 * Retrieves character details component for a given uuid.
 *
 * Returns the character_details_t or null if not found.
**/
character_details_t * get_character_details(components_t * components, char * uuid) {
	return (character_details_t *) hash_table_get(components->character_details, uuid);
}


/**
 * Updates registered character details.  This is currently no-op.
**/
void update_character_details(game_t * game) {
}