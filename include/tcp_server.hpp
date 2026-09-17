#pragma once

#include "kv_store.hpp"

class TCPServer {
private:
    int server;
    int port;
    KVStore &store;

    void handle_client(int client);

public:
    TCPServer(int port, KVStore &store);

    void start();
};