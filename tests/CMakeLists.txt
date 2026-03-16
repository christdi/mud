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
