#pragma once

#include "kv_store.hpp"
#include "thread_pool.hpp"
#include <atomic>
#include <string>

class TCPServer {
private:
    int server;
    int port;

    KVStore &store;
    ThreadPool thread_pool;

    atomic<size_t> active_clients{0};

    static constexpr size_t MAX_CLIENTS = 8;
    static constexpr size_t MAX_REQUEST_SIZE = 64*1024;

    void handle_client(int client);

public:
    TCPServer(int port, KVStore &store);

    void start();
};