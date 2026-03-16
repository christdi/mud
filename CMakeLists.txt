# CMake configuration
cmake_minimum_required (VERSION 2.8.12)

enable_language(C)

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED)
set (PROJECT_NAME mud)
set (CMAKE_C_CLANG_TIDY "clang-tidy" "-checks=clang-analyzer-*, concurrency-*, misc-*, performance-*, portability-*, readability-*, -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling, -misc-unused-parameters, -clang-diagnostic-unused-parameter, -readability-non-const-parameter, -readability-magic-numbers" "-header-filter=src/.*")
# "-warnings-as-errors=*, -readability-non-const-parameter, -misc-unused-parameters, -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling"

project (${PROJECT_NAME} C)

find_package(Lua51 REQUIRED)

# Build configuration
add_definitions("-Wall -pedantic")

include_directories(${PROJECT_SOURCE_DIR}/include ${LUA_INCLUDE_DIR})

# libmud: all sources except main.c, shared by the game executable and tests
file(GLOB_RECURSE LIB_SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src/*.c)
list(REMOVE_ITEM LIB_SRC_FILES "src/main.c")

add_library(libmud STATIC ${LIB_SRC_FILES})
target_link_libraries(libmud uuid sqlite3 ssl crypto ${LUA_LIBRARIES} ${CMAKE_DL_LIBS})

# Game executable
add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} libmud)
install(TARGETS ${PROJECT_NAME} DESTINATION ..)

# Tests
enable_testing()
add_subdirectory(tests)
