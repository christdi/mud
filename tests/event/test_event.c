#include <stdlib.h>

#include "unity.h"

#include "mock_player.h"
#include "mud/data/hash_table.h"
#include "mud/event.h"

static int deallocator_call_count = 0;

static void counting_deallocator(void* data) {
  (void)data;
  deallocator_call_count++;
}

static void noop_deallocator(void* data) {
  (void)data;
}

/* A newly allocated event_t is not NULL. */
void test_event_new_returns_non_null(void) {
  event_t* event = event_new_event_t(LUA_EVENT, NULL, NULL);
  TEST_ASSERT_NOT_NULL(event);
  event_free_event_t(event);
}

/* The type field passed to event_new_event_t is stored on the struct. */
void test_event_new_stores_type(void) {
  event_t* event = event_new_event_t(LUA_EVENT, NULL, NULL);
  TEST_ASSERT_EQUAL_INT(LUA_EVENT, event->type);
  event_free_event_t(event);
}

/* The data pointer passed to event_new_event_t is stored on the struct. */
void test_event_new_stores_data(void) {
  int payload = 42;
  event_t* event = event_new_event_t(LUA_EVENT, &payload, noop_deallocator);
  TEST_ASSERT_EQUAL_PTR(&payload, event->data);
  event_free_event_t(event);
}

/* Freeing an event with data and a deallocator invokes the deallocator. */
void test_event_free_calls_deallocator(void) {
  deallocator_call_count = 0;
  int payload = 1;
  event_t* event = event_new_event_t(LUA_EVENT, &payload, counting_deallocator);
  event_free_event_t(event);
  TEST_ASSERT_EQUAL_INT(1, deallocator_call_count);
}

/* Freeing an event with no data does not crash even without a deallocator. */
void test_event_free_null_data_no_crash(void) {
  event_t* event = event_new_event_t(LUA_EVENT, NULL, NULL);
  event_free_event_t(event);
}

/* A freshly allocated event broker is not NULL. */
void test_event_broker_new_returns_non_null(void) {
  event_broker_t* broker = event_new_event_broker_t();
  TEST_ASSERT_NOT_NULL(broker);
  event_free_event_broker_t(broker);
}

/* An empty broker reports no pending events. */
void test_event_has_events_false_when_empty(void) {
  event_broker_t* broker = event_new_event_broker_t();
  TEST_ASSERT_FALSE(event_has_events(broker));
  event_free_event_broker_t(broker);
}

/* Submitting an event causes the broker to report pending events. */
void test_event_has_events_true_after_submit(void) {
  event_broker_t* broker = event_new_event_broker_t();
  event_t* event = event_new_event_t(LUA_EVENT, NULL, NULL);
  event_submit_event(broker, event);
  TEST_ASSERT_TRUE(event_has_events(broker));
  event_free_event_broker_t(broker);
}

/* Each submitted event increases the pending count by one. */
void test_event_submit_increments_count(void) {
  event_broker_t* broker = event_new_event_broker_t();
  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));
  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));
  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));
  TEST_ASSERT_EQUAL_INT(3, list_size(broker->events));
  event_free_event_broker_t(broker);
}

/* After dispatching all events the broker has no remaining pending events. */
void test_event_dispatch_clears_broker(void) {
  event_broker_t* broker = event_new_event_broker_t();
  hash_table_t* entities = create_hash_table_t();
  hash_table_t* players = create_hash_table_t();

  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));
  event_dispatch_events(broker, NULL, entities, players);

  TEST_ASSERT_FALSE(event_has_events(broker));

  event_free_event_broker_t(broker);
  free_hash_table_t(entities);
  free_hash_table_t(players);
}

/* player_on_event is called once when there is one player and one event. */
void test_event_dispatch_calls_player_on_event(void) {
  mock_player_reset();

  event_broker_t* broker = event_new_event_broker_t();
  hash_table_t* entities = create_hash_table_t();
  hash_table_t* players = create_hash_table_t();

  player_t player = {0};
  hash_table_insert(players, "player1", &player);
  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));

  event_dispatch_events(broker, NULL, entities, players);

  TEST_ASSERT_EQUAL_INT(1, player_on_event_fake.call_count);
  TEST_ASSERT_EQUAL_PTR(&player, player_on_event_fake.arg0_history[0]);

  event_free_event_broker_t(broker);
  free_hash_table_t(entities);
  free_hash_table_t(players);
}

/* player_on_event is called once per player when multiple players are registered. */
void test_event_dispatch_calls_each_player(void) {
  mock_player_reset();

  event_broker_t* broker = event_new_event_broker_t();
  hash_table_t* entities = create_hash_table_t();
  hash_table_t* players = create_hash_table_t();

  player_t player1 = {0};
  player_t player2 = {0};
  hash_table_insert(players, "player1", &player1);
  hash_table_insert(players, "player2", &player2);
  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));

  event_dispatch_events(broker, NULL, entities, players);

  TEST_ASSERT_EQUAL_INT(2, player_on_event_fake.call_count);

  event_free_event_broker_t(broker);
  free_hash_table_t(entities);
  free_hash_table_t(players);
}

/* player_on_event is called once per event when multiple events are queued. */
void test_event_dispatch_calls_once_per_event(void) {
  mock_player_reset();

  event_broker_t* broker = event_new_event_broker_t();
  hash_table_t* entities = create_hash_table_t();
  hash_table_t* players = create_hash_table_t();

  player_t player = {0};
  hash_table_insert(players, "player1", &player);
  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));
  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));

  event_dispatch_events(broker, NULL, entities, players);

  TEST_ASSERT_EQUAL_INT(2, player_on_event_fake.call_count);

  event_free_event_broker_t(broker);
  free_hash_table_t(entities);
  free_hash_table_t(players);
}

/* No calls are made when the player table is empty. */
void test_event_dispatch_no_players_no_calls(void) {
  mock_player_reset();

  event_broker_t* broker = event_new_event_broker_t();
  hash_table_t* entities = create_hash_table_t();
  hash_table_t* players = create_hash_table_t();

  event_submit_event(broker, event_new_event_t(LUA_EVENT, NULL, NULL));
  event_dispatch_events(broker, NULL, entities, players);

  TEST_ASSERT_EQUAL_INT(0, player_on_event_fake.call_count);

  event_free_event_broker_t(broker);
  free_hash_table_t(entities);
  free_hash_table_t(players);
}

void run_event_tests(void) {
  RUN_TEST(test_event_new_returns_non_null);
  RUN_TEST(test_event_new_stores_type);
  RUN_TEST(test_event_new_stores_data);
  RUN_TEST(test_event_free_calls_deallocator);
  RUN_TEST(test_event_free_null_data_no_crash);
  RUN_TEST(test_event_broker_new_returns_non_null);
  RUN_TEST(test_event_has_events_false_when_empty);
  RUN_TEST(test_event_has_events_true_after_submit);
  RUN_TEST(test_event_submit_increments_count);
  RUN_TEST(test_event_dispatch_clears_broker);
  RUN_TEST(test_event_dispatch_calls_player_on_event);
  RUN_TEST(test_event_dispatch_calls_each_player);
  RUN_TEST(test_event_dispatch_calls_once_per_event);
  RUN_TEST(test_event_dispatch_no_players_no_calls);
}
