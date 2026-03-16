# Suppress -Wpedantic for all test targets: FFF macros emit extra semicolons
# that are valid GCC extensions but not strict ISO C.
add_compile_options(-Wno-pedantic)

# Disable clang-tidy for all test targets (vendor code and test macros produce noise)
set(CMAKE_C_CLANG_TIDY "")

# test_linked_list
add_executable(test_linked_list
  vendor/unity.c
  data/test_linked_list.c
  ${PROJECT_SOURCE_DIR}/src/log.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/linked_list.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/node.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/iterator.c
)

target_include_directories(test_linked_list PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

target_link_libraries(test_linked_list pthread)

add_test(NAME test_linked_list COMMAND test_linked_list)

# test_event
add_executable(test_event
  vendor/unity.c
  event/test_event.c
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
)

target_link_libraries(test_event pthread)

add_test(NAME test_event COMMAND test_event)
