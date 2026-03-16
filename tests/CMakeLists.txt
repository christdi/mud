add_executable(test_mud
  vendor/unity.c
  mocks/mock_player.c
  data/test_linked_list.c
  event/test_event.c
  test_runner.c
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

target_include_directories(test_mud PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(test_mud pthread)

# Suppress -Wpedantic for the test target: FFF macros emit extra semicolons
# that are valid GCC extensions but not strict ISO C.
target_compile_options(test_mud PRIVATE -Wno-pedantic)

# Disable clang-tidy for tests (vendor code and test macros produce noise)
set_target_properties(test_mud PROPERTIES C_CLANG_TIDY "")

add_test(NAME test_mud COMMAND test_mud)
