cmake_minimum_required(VERSION 3.20)
project(concurrent_kv_store)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(kvstore
    src/main.cpp
    src/kv_store.cpp
    src/tcp_server.cpp
    src/thread_pool.cpp
    src/protocol/command.cpp
    src/protocol/parser.cpp
)

target_include_directories(kvstore PRIVATE include)
target_link_libraries(kvstore PRIVATE Threads::Threads)

enable_testing()

function(add_unit_test target source)
    add_executable(${target} ${source} ${ARGN})
    target_include_directories(${target} PRIVATE include tests/unit)
    target_link_libraries(${target} PRIVATE Threads::Threads)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_unit_test(
    kv_store_test
    tests/unit/kv_store_test.cpp
    src/kv_store.cpp
)

add_unit_test(
    kv_store_concurrency_test
    tests/unit/kv_store_concurrency_test.cpp
    src/kv_store.cpp
)

add_unit_test(
    parser_test
    tests/unit/parser_test.cpp
    src/protocol/parser.cpp
    src/protocol/command.cpp
)

add_unit_test(
    thread_pool_test
    tests/unit/thread_pool_test.cpp
    src/thread_pool.cpp
)