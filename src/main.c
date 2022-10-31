#include <stdio.h>

#include "mud/game.h"

#include "mud/data/array_list/array_list.h"

DECLARE_LIST(int_array, int)

static bool is_even(int value);

/**
 * Entry point for the application.  Will exit if unable to load to
 * configuration or initialise logging.  Otherwise, starts the game.
 **/
int main(int argc, char* argv[]) {
  int_array_t array;
  int_array_init(&array);

  for (int i = 0; i < 100; i++) {
    int_array_push_back(&array, i);    
  }

  printf("Array size: [%ld], capacity: [%ld]\n\r", array.size, array.capacity);

  for (size_t i = 0; i < array.size; i++) {
    printf("Array index [%ld], value [%d]\n\r", i, int_array_at(&array, i));
  }

  int_array_t even = int_array_filter(&array, is_even);

  printf("Even size: [%ld], capacity: [%ld]\n\r", even.size, even.capacity);

  for (size_t i = 0; i < even.size; i++) {
    printf("Even index [%ld], value [%d]\n\r", i, int_array_at(&even, i));
  }

  int_array_free(&even);

  int_array_free(&array);

  // if (start_game(argc, argv) != 0) {
  //   return -1;
  // }

  return 0;
}

bool is_even(int value) {
  return value % 2 == 0;
}
