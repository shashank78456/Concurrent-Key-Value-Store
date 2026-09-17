#include "tcp_server.hpp"
#include "protocol/parser.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

TCPServer::TCPServer(int port, KVStore &store) : server(-1), port(port), store(store), thread_pool(8) {}

void TCPServer::handle_client(int client) {
    char buffer[4096];

    while(true) {
        ssize_t bytes_read = recv(client, buffer, sizeof(buffer), 0);

        if(bytes_read<=0)
            break;

        Command command = Parser::parse(string(buffer, bytes_read));

        string response;

        switch(command.type) {
            case CommandType::SET: {
                store.set(command.key, command.value);
                response = "OK\n";
                break;
            }

            case CommandType::GET: {
                optional<string> value = store.get(command.key);
                response = value ? *value + "\n" : "NOT FOUND\n";
                break;
            }

            case CommandType::DELETE: {
                response = store.remove(command.key) ? "OK\n" : "NOT FOUND\n";
                break;

            }

            case CommandType::INVALID: {
                response = "ERROR\n";
                break;
            }
        }

        send(client, response.data(), response.size(), 0);
    }

    close(client);
}

void TCPServer::start() {
    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server<0)
        throw runtime_error("Failed to create server");
    
    int option = 1;
    
    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))<0) {
        close(server);
        throw runtime_error("Failed to set socket options");
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // inet_addr("127.0.0.1");
    server_address.sin_port = htons(port);

    if(::bind(server, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address))<0) {
        close(server);
        throw runtime_error("Failed to bind socket");
    }

    if(listen(server, 128)<0) {
        close(server);
        throw runtime_error("Failed to listen");
    }

    cout << "Server listening on port: " << port << '\n';

    while(true) {
        int client = accept(server, nullptr, nullptr);

        if(client < 0) {
            cerr << "Failed to accept client\n";
            continue;
        }

        // thread(&TCPServer::handle_client, this, client).detach();

        thread_pool.enqueue([this, client]() {
            handle_client(client);
        });
    }
}