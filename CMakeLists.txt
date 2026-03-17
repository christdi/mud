cmake_minimum_required(VERSION 3.5)

project(mud C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_CLANG_TIDY
  "clang-tidy"
  "-checks=clang-analyzer-*, concurrency-*, misc-*, performance-*, portability-*, readability-*, -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling, -misc-unused-parameters, -clang-diagnostic-unused-parameter, -readability-non-const-parameter, -readability-magic-numbers"
  "-header-filter=src/.*")

find_package(Lua 5.3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_compile_options(-Wall -pedantic)

set(LIBMUD_SOURCES
  src/action.c
  src/bsd/string.c
  src/command.c
  src/config.c
  src/data/deallocate.c
  src/data/hash_table/hash_iterator.c
  src/data/hash_table/hash_node.c
  src/data/hash_table/hash_table.c
  src/data/linked_list/iterator.c
  src/data/linked_list/linked_list.c
  src/data/linked_list/node.c
  src/data/queue/queue.c
  src/db.c
  src/ecs/archetype.c
  src/ecs/component.c
  src/ecs/entity.c
  src/ecs/system.c
  src/event.c
  src/game.c
  src/json.c
  src/log.c
  src/lua/common.c
  src/lua/db_api.c
  src/lua/game_api.c
  src/lua/hooks.c
  src/lua/log_api.c
  src/lua/player_api.c
  src/lua/ref.c
  src/lua/script.c
  src/lua/script_api.c
  src/lua/struct.c
  src/network/callback.c
  src/network/client.c
  src/network/gmcp.c
  src/network/network.c
  src/network/protocol.c
  src/network/server.c
  src/network/telnet.c
  src/player.c
  src/task.c
  src/util/mudstring.c
  src/util/mudhash.c
  src/util/muduuid.c
)

add_library(libmud STATIC ${LIBMUD_SOURCES})

target_include_directories(libmud
  PUBLIC  ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${LUA_INCLUDE_DIR}
)

target_link_libraries(libmud
  uuid
  sqlite3
  OpenSSL::SSL
  OpenSSL::Crypto
  Threads::Threads
  ${LUA_LIBRARIES}
  ${CMAKE_DL_LIBS}
)

add_executable(mud src/main.c)
target_link_libraries(mud libmud)
install(TARGETS mud DESTINATION ..)

enable_testing()
add_subdirectory(tests)
