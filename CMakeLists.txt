# CMake configuration
cmake_minimum_required (VERSION 2.8.12)

enable_language(C)

set (PROJECT_NAME mud)
set (LIB_NAME mudlib)

project (${PROJECT_NAME} C)

# Build configuration
add_definitions("-Wall -pedantic")

# Library configuration
file(GLOB_RECURSE SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src/*.c)
list(REMOVE_ITEM SRC_FILES "src/main.c")
add_library(${LIB_NAME} ${SRC_FILES})

# Executable configuration
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} ${LIB_NAME} zlog uuid)