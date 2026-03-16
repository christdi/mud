add_executable(test_mud
  vendor/unity.c
  data/test_linked_list.c
  test_runner.c
)

target_include_directories(test_mud PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

target_link_libraries(test_mud libmud)

# Disable clang-tidy for tests (vendor code and test macros produce noise)
set_target_properties(test_mud PROPERTIES C_CLANG_TIDY "")

add_test(NAME test_mud COMMAND test_mud)

# test_event: event.c with link-seam mock for player_on_event.
# Links only the minimal transitive dependencies of event.c — no libmud —
# so that mock_player.c can replace player.c without a duplicate symbol conflict.
add_executable(test_event
  vendor/unity.c
  mocks/mock_player.c
  event/test_event.c
  event/test_event_runner.c
  ${PROJECT_SOURCE_DIR}/src/event.c
  ${PROJECT_SOURCE_DIR}/src/log.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/linked_list.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/node.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/iterator.c
  ${PROJECT_SOURCE_DIR}/src/data/hash_table/hash_table.c
  ${PROJECT_SOURCE_DIR}/src/data/hash_table/hash_node.c
  ${PROJECT_SOURCE_DIR}/src/data/hash_table/hash_iterator.c
  ${PROJECT_SOURCE_DIR}/src/data/queue/queue.c
)

target_include_directories(test_event PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_event pthread)

set_target_properties(test_event PROPERTIES C_CLANG_TIDY "")

add_test(NAME test_event COMMAND test_event)
