#include "tcp_server.hpp"

#include <iostream>
#include <exception>

int main() {
    try {
        KVStore kvstore;

        TCPServer server(6379, kvstore);
        server.start();
    }
    catch(const exception &e) {
        cerr << "Server error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}