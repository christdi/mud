#include "fff.h"
#include "unity.h"

DEFINE_FFF_GLOBALS;

void run_linked_list_tests(void);
void run_event_tests(void);

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  run_linked_list_tests();
  run_event_tests();

  return UNITY_END();
}
