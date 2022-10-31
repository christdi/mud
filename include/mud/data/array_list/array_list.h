#ifndef MUD_DATA_ARRAY_LIST_ARRAY_LIST_H
#define MUD_DATA_ARRAY_LIST_ARRAY_LIST_H

#include <stdlib.h>
#include <stddef.h>

#define DEFAULT_CAPACITY 16

#define DECLARE_LIST(name, type)                                                  \
typedef struct name {                                                             \
  type* items;                                                                    \
  size_t capacity;                                                                \
  size_t size;                                                                    \
} name##_t;                                                                       \
                                                                                  \
void name##_init(name##_t* array) {                                               \
  array->items = calloc(sizeof(type), DEFAULT_CAPACITY);                          \
  array->capacity = DEFAULT_CAPACITY;                                             \
  array->size = 0;                                                                \
}                                                                                 \
                                                                                  \
void name##_free(name##_t* array) {                                               \
  free(array->items);                                                             \
}                                                                                 \
                                                                                  \
void name##_push_back(name##_t* array, type data) {                               \
  if (array->size == array->capacity) {                                           \
    array->capacity *= 2;                                                         \
    array->items = realloc(array->items, sizeof(type) * array->capacity);         \
  }                                                                               \
                                                                                  \
  array->items[array->size] = data;                                               \
  array->size = array->size + 1;                                                  \
}                                                                                 \
                                                                                  \
void name##_remove(name##_t* array, size_t index) {                               \
  while(index < array->size) {                                                    \
    array->items[index] = array->items[index + 1];                                \
    index++;                                                                      \
  }                                                                               \
                                                                                  \
  array->size--;                                                                  \
}                                                                                 \
                                                                                  \
type name##_at(name##_t* array, size_t index) {                                   \
  return array->items[index];                                                     \
}                                                                                 \

#endif