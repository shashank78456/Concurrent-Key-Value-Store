cmake_minimum_required(VERSION 3.20)
project(concurrent_kv_store)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kvstore
    src/main.cpp
    src/kv_store.cpp
    src/tcp_server.cpp
    src/thread_pool.cpp
    src/protocol/command.cpp
    src/protocol/parser.cpp
)

target_include_directories(kvstore PRIVATE include)