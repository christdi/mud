#include "mud/structure/list.h"
#include "mud/structure/node.h"

#include <assert.h>
#include <stdlib.h>

list_t * list_new() {
    list_t * list = calloc(1, sizeof * list);

    list->first = NULL;
    list->last = NULL;

    pthread_mutex_init(&list->mutex, NULL);
	
	return list;
}

void list_free(list_t * list) {
    assert(list);

    node_t * node = NULL;


    list_first(list, &node);


    while(node != NULL) {
        node_t * next = NULL;

        list_next(list, &next);
        node_free(node);

        node = next;
    }

    list->first = NULL;
    list->last = NULL;
    pthread_mutex_destroy(&list->mutex);
    
    free(list);
    list = NULL;
}

const int list_insert(list_t * list, node_t * node) {


    assert(list);
    assert(node);

    if ( !list ) {
        return -1;
    }

    pthread_mutex_lock(&list->mutex);

    if ( !list->first ) {
        list->first = node;

        list->last = node;


        pthread_mutex_unlock(&list->mutex);

        return 0;
    }

    node_t * last = list->last;

    last->next = node;

    node->prev = last;

    list->last = node;

    pthread_mutex_unlock(&list->mutex);


    return 0;
}

const int list_remove(list_t * list, node_t * node) {

    assert(list);
    assert(node);

    pthread_mutex_lock(&list->mutex);

    node_t * search = list->first;

    while ( search ) {
        node_t * current = search;

        if ( current == node ) {
            node_t * previous = current->prev;
            node_t * next = current->next;

            if ( current == list->first ) {
                list->first = next;
            }

            if ( current == list->last ) {
                list->last = previous;
            }

            if (previous) {
                previous->next = next;
            }

            if (next) {
                next->prev = previous;
            }
        }

        search = current->next;
    }

    pthread_mutex_unlock(&list->mutex);


    return 0;
}

void list_next(list_t * list, node_t ** node) {
    assert(list);
    assert(node);

    pthread_mutex_lock(&list->mutex);

    if ( !(*node)->next ) {
        *node = NULL;
    } else {
        *node = (*node)->next;
    }

    pthread_mutex_unlock(&list->mutex);
}

void list_prev(list_t * list, node_t ** node) {
    assert(list);
    assert(node);

    pthread_mutex_lock(&list->mutex);

    if ( !(*node)->prev ) {
        *node = NULL;
    } else {
        *node = (*node)->prev;
    }

    pthread_mutex_unlock(&list->mutex);
}

void list_first(list_t * list, node_t ** node) {
    assert(list);
    assert(node);

    pthread_mutex_lock(&list->mutex);

    *node = list->first;

    pthread_mutex_unlock(&list->mutex);
}

void list_last(list_t * list, node_t ** node) {
    assert(list);
    assert(node);

    pthread_mutex_lock(&list->mutex);

    *node = list->last;

    pthread_mutex_unlock(&list->mutex);
}

const int list_clear(list_t * list) {
    assert(list);

    pthread_mutex_lock(&list->mutex);

    node_t * node = list->first;

    while ( node != list->last ) {
        node_t * next = node->next;

        node->prev = 0;
        node->next = 0;

        node = next;
    }

    list->first = 0;
    list->last = 0;

    pthread_mutex_unlock(&list->mutex);

    return 0;
}

const int list_count(list_t * list) {
    assert(list);

    pthread_mutex_lock(&list->mutex);

    int count = 0;

    node_t * node = list->first;

    while ( node != list->last ) {
        count++;

        node = node->next;
    }

    pthread_mutex_unlock(&list->mutex);

    return count;
}