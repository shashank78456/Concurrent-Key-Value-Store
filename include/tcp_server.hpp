#pragma once

#include "kv_store.hpp"
#include "thread_pool.hpp"

class TCPServer {
private:
    int server;
    int port;
    KVStore &store;

    ThreadPool thread_pool;

    void handle_client(int client);

public:
    TCPServer(int port, KVStore &store);

    void start();
};