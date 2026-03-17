#include <stdlib.h>

#include "fff.h"
#include "unity.h"

#include "mud/lua/hooks_api.h"
#include "mud/lua/ref.h"

DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(lua_free_lua_ref_t, lua_ref_t*);

/* lua_new_hooks_t returns a non-null pointer. */
void test_hooks_new_returns_non_null(void) {
  lua_hooks_t* hooks = lua_new_hooks_t();
  TEST_ASSERT_NOT_NULL(hooks);
  free(hooks);
}

/* All hook refs are NULL immediately after allocation. */
void test_hooks_new_all_fields_null(void) {
  lua_hooks_t* hooks = lua_new_hooks_t();

  TEST_ASSERT_NULL(hooks->on_startup);
  TEST_ASSERT_NULL(hooks->on_shutdown);
  TEST_ASSERT_NULL(hooks->on_entities_loaded);
  TEST_ASSERT_NULL(hooks->on_commands_loaded);
  TEST_ASSERT_NULL(hooks->on_command_groups_loaded);
  TEST_ASSERT_NULL(hooks->on_actions_loaded);
  TEST_ASSERT_NULL(hooks->on_player_connected);
  TEST_ASSERT_NULL(hooks->on_player_disconnected);
  TEST_ASSERT_NULL(hooks->on_player_input);

  free(hooks);
}

/* lua_free_hooks_t does not crash when all refs are NULL. */
void test_hooks_free_null_refs_no_crash(void) {
  lua_hooks_t* hooks = lua_new_hooks_t();
  lua_free_hooks_t(hooks);
}

/* lua_free_hooks_t calls lua_free_lua_ref_t once per non-null ref. */
void test_hooks_free_calls_free_ref_for_each_populated_ref(void) {
  RESET_FAKE(lua_free_lua_ref_t);

  lua_hooks_t* hooks = lua_new_hooks_t();

  lua_ref_t ref_a = { .state = NULL, .ref = 0 };
  lua_ref_t ref_b = { .state = NULL, .ref = 0 };

  hooks->on_startup = &ref_a;
  hooks->on_player_connected = &ref_b;

  lua_free_hooks_t(hooks);

  TEST_ASSERT_EQUAL_INT(9, lua_free_lua_ref_t_fake.call_count);
}

/* lua_free_hooks_t passes each stored ref pointer to lua_free_lua_ref_t. */
void test_hooks_free_passes_correct_ref_pointers(void) {
  RESET_FAKE(lua_free_lua_ref_t);

  lua_hooks_t* hooks = lua_new_hooks_t();

  lua_ref_t ref = { .state = NULL, .ref = 0 };
  hooks->on_shutdown = &ref;

  lua_free_hooks_t(hooks);

  int found = 0;

  for (int i = 0; i < lua_free_lua_ref_t_fake.call_count; i++) {
    if (lua_free_lua_ref_t_fake.arg0_history[i] == &ref) {
      found = 1;
      break;
    }
  }

  TEST_ASSERT_TRUE(found);
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_hooks_new_returns_non_null);
  RUN_TEST(test_hooks_new_all_fields_null);
  RUN_TEST(test_hooks_free_null_refs_no_crash);
  RUN_TEST(test_hooks_free_calls_free_ref_for_each_populated_ref);
  RUN_TEST(test_hooks_free_passes_correct_ref_pointers);
  return UNITY_END();
}
