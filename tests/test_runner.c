#include "unity.h"

void run_linked_list_tests(void);

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  run_linked_list_tests();

  return UNITY_END();
}
