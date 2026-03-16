#include <stdlib.h>

#include "unity.h"

#include "mud/data/linked_list/linked_list.h"

static int deallocator_call_count = 0;

static void counting_deallocator(void* value) {
  (void)value;
  deallocator_call_count++;
}

static int always_true_predicate(void* value) {
  (void)value;
  return 1;
}

static int always_false_predicate(void* value) {
  (void)value;
  return 0;
}

void test_list_create_and_free(void) {
  linked_list_t* list = create_linked_list_t();
  TEST_ASSERT_NOT_NULL(list);
  free_linked_list_t(list);
}

void test_list_free_null_is_safe(void) {
  free_linked_list_t(NULL);
}

void test_list_size_empty(void) {
  linked_list_t* list = create_linked_list_t();
  TEST_ASSERT_EQUAL_INT(0, list_size(list));
  free_linked_list_t(list);
}

void test_list_add_single(void) {
  linked_list_t* list = create_linked_list_t();
  int value = 42;
  list_add(list, &value);
  TEST_ASSERT_EQUAL_INT(1, list_size(list));
  free_linked_list_t(list);
}

void test_list_add_multiple(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);
  TEST_ASSERT_EQUAL_INT(3, list_size(list));
  free_linked_list_t(list);
}

void test_list_contains_present(void) {
  linked_list_t* list = create_linked_list_t();
  int value = 42;
  list_add(list, &value);
  TEST_ASSERT_TRUE(list_contains(list, &value));
  free_linked_list_t(list);
}

void test_list_contains_absent(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2;
  list_add(list, &a);
  TEST_ASSERT_FALSE(list_contains(list, &b));
  free_linked_list_t(list);
}

void test_list_remove_reduces_size(void) {
  linked_list_t* list = create_linked_list_t();
  int value = 42;
  list_add(list, &value);
  list_remove(list, &value);
  TEST_ASSERT_EQUAL_INT(0, list_size(list));
  free_linked_list_t(list);
}

void test_list_remove_calls_deallocator(void) {
  deallocator_call_count = 0;
  linked_list_t* list = create_linked_list_t();
  list->deallocator = counting_deallocator;
  int value = 42;
  list_add(list, &value);
  list_remove(list, &value);
  TEST_ASSERT_EQUAL_INT(1, deallocator_call_count);
  free_linked_list_t(list);
}

void test_list_remove_from_front(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);
  list_remove(list, &a);
  TEST_ASSERT_EQUAL_INT(2, list_size(list));
  TEST_ASSERT_FALSE(list_contains(list, &a));
  free_linked_list_t(list);
}

void test_list_remove_from_end(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);
  list_remove(list, &c);
  TEST_ASSERT_EQUAL_INT(2, list_size(list));
  TEST_ASSERT_FALSE(list_contains(list, &c));
  free_linked_list_t(list);
}

void test_list_remove_from_middle(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);
  list_remove(list, &b);
  TEST_ASSERT_EQUAL_INT(2, list_size(list));
  TEST_ASSERT_FALSE(list_contains(list, &b));
  free_linked_list_t(list);
}

void test_list_steal_does_not_call_deallocator(void) {
  deallocator_call_count = 0;
  linked_list_t* list = create_linked_list_t();
  list->deallocator = counting_deallocator;
  int value = 42;
  list_add(list, &value);
  list_steal(list, &value);
  TEST_ASSERT_EQUAL_INT(0, list_size(list));
  TEST_ASSERT_EQUAL_INT(0, deallocator_call_count);
  free_linked_list_t(list);
}

void test_list_clear_empties_list(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);
  list_clear(list);
  TEST_ASSERT_EQUAL_INT(0, list_size(list));
  free_linked_list_t(list);
}

void test_list_clear_calls_deallocators(void) {
  deallocator_call_count = 0;
  linked_list_t* list = create_linked_list_t();
  list->deallocator = counting_deallocator;
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);
  list_clear(list);
  TEST_ASSERT_EQUAL_INT(3, deallocator_call_count);
  free_linked_list_t(list);
}

void test_list_iterator_traverses_all(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);

  int count = 0;
  it_t it = list_begin(list);

  while (it_get(it) != NULL) {
    count++;
    it = it_next(it);
  }

  TEST_ASSERT_EQUAL_INT(3, count);
  free_linked_list_t(list);
}

void test_list_iterator_order(void) {
  linked_list_t* list = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(list, &a);
  list_add(list, &b);
  list_add(list, &c);

  it_t it = list_begin(list);
  TEST_ASSERT_EQUAL_PTR(&a, it_get(it));
  it = it_next(it);
  TEST_ASSERT_EQUAL_PTR(&b, it_get(it));
  it = it_next(it);
  TEST_ASSERT_EQUAL_PTR(&c, it_get(it));

  free_linked_list_t(list);
}

void test_list_extract_all_matching(void) {
  linked_list_t* src = create_linked_list_t();
  linked_list_t* dst = create_linked_list_t();
  int a = 1, b = 2, c = 3;
  list_add(src, &a);
  list_add(src, &b);
  list_add(src, &c);

  list_extract(src, dst, always_true_predicate);

  TEST_ASSERT_EQUAL_INT(0, list_size(src));
  TEST_ASSERT_EQUAL_INT(3, list_size(dst));

  free_linked_list_t(src);
  free_linked_list_t(dst);
}

void test_list_extract_none_matching(void) {
  linked_list_t* src = create_linked_list_t();
  linked_list_t* dst = create_linked_list_t();
  int a = 1, b = 2;
  list_add(src, &a);
  list_add(src, &b);

  list_extract(src, dst, always_false_predicate);

  TEST_ASSERT_EQUAL_INT(2, list_size(src));
  TEST_ASSERT_EQUAL_INT(0, list_size(dst));

  free_linked_list_t(src);
  free_linked_list_t(dst);
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_list_create_and_free);
  RUN_TEST(test_list_free_null_is_safe);
  RUN_TEST(test_list_size_empty);
  RUN_TEST(test_list_add_single);
  RUN_TEST(test_list_add_multiple);
  RUN_TEST(test_list_contains_present);
  RUN_TEST(test_list_contains_absent);
  RUN_TEST(test_list_remove_reduces_size);
  RUN_TEST(test_list_remove_calls_deallocator);
  RUN_TEST(test_list_remove_from_front);
  RUN_TEST(test_list_remove_from_end);
  RUN_TEST(test_list_remove_from_middle);
  RUN_TEST(test_list_steal_does_not_call_deallocator);
  RUN_TEST(test_list_clear_empties_list);
  RUN_TEST(test_list_clear_calls_deallocators);
  RUN_TEST(test_list_iterator_traverses_all);
  RUN_TEST(test_list_iterator_order);
  RUN_TEST(test_list_extract_all_matching);
  RUN_TEST(test_list_extract_none_matching);
  return UNITY_END();
}
