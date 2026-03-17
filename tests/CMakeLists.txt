# Suppress -Wpedantic for all test targets: FFF macros emit extra semicolons
# that are valid GCC extensions but not strict ISO C.
add_compile_options(-Wno-pedantic)

# Disable clang-tidy for all test targets (vendor code and test macros produce noise)
set(CMAKE_C_CLANG_TIDY "")

# Creates a self-contained test binary registered with ctest.
# Usage: mud_add_test(<name> <sources...>)
function(mud_add_test name)
  add_executable(${name} ${ARGN})
  target_include_directories(${name} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
  )
  target_link_libraries(${name} Threads::Threads)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

mud_add_test(test_linked_list
  vendor/unity.c
  data/test_linked_list.c
  ${PROJECT_SOURCE_DIR}/src/log.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/linked_list.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/node.c
  ${PROJECT_SOURCE_DIR}/src/data/linked_list/iterator.c
)

mud_add_test(test_event
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
