#include "unity.h"

void run_event_tests(void);

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  run_event_tests();

  return UNITY_END();
}
