# CMake configuration
cmake_minimum_required (VERSION 2.8.12)

enable_language(C)

set (CMAKE_C_STANDARD 11)
set (CMAKE_C_STANDARD_REQUIRED)
set (PROJECT_NAME mud)
set (CMAKE_C_CLANG_TIDY "clang-tidy" "-checks=clang-analyzer-*, concurrency-*, misc-*, performance-*, portability-*, readability-*, -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling, -misc-unused-parameters, -clang-diagnostic-unused-parameter, -readability-non-const-parameter" "-header-filter=*")
# "-warnings-as-errors=*, -readability-non-const-parameter, -misc-unused-parameters, -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling"

project (${PROJECT_NAME} C)

find_package(Lua51 REQUIRED)

# Build configuration
add_definitions("-Wall -pedantic")

include_directories(${PROJECT_SOURCE_DIR}/include ${LUA_INCLUDE_DIR})

# Executable configuration
file(GLOB_RECURSE SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} uuid sqlite3 ssl crypto ${LUA_LIBRARIES} ${CMAKE_DL_LIBS})
