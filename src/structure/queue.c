#include "mud/structure/queue.h"

#include <stdlib.h>

queue_t * queue_new() {
	queue_t * queue = calloc(1, sizeof * queue);

	queue->list = list_new();

	return queue;
}

const int queue_push(queue_t * queue, node_t * node) {
	if ( !queue || !node ) {
		return -1;
	}

	return list_insert(queue->list, node);
}

const int queue_pop(queue_t * queue, node_t * node) {
	if ( !queue ) {
		return -1;
	}

	if ( list_last(queue->list, node) != 0 ) {
		return -1;
	}

	if ( list_remove(queue->list, node) != 0 ) {
		return -1;
	}

	return 0;
}

const int queue_clear(queue_t * queue) {
	if ( !queue ) {
		return -1;
	}

	return list_clear(queue->list);
}

const int queue_is_empty(queue_t * queue) {
	if ( !queue ) {
		return -1;
	}

	return list_count(queue->list) > 0 ? 1 : 0;
}

void queue_free(queue_t * queue) {
	if ( queue ) {
		list_free(queue->list);

		free(queue);
	}
}