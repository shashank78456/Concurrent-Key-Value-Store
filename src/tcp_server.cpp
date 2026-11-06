#include "tcp_server.hpp"
#include "protocol/parser.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <cerrno>

using namespace std;

bool send_all(int client, const string &response) {
    size_t total_sent = 0;

    while(total_sent < response.size()) {
        int flags = 0;

#ifdef MSG_NOSIGNAL
        flags |= MSG_NOSIGNAL;
#endif

        ssize_t bytes_sent = send(client, response.data() + total_sent, response.size() - total_sent, flags);
        
        if(bytes_sent<=0)
            return false;
        
        total_sent+=bytes_sent;
    }   

    return true;
}

TCPServer::TCPServer(int port, KVStore &store) : server(-1), port(port), store(store), thread_pool(8) {}

void TCPServer::handle_client(int client) {
    char buffer[4096];
    string pending;

    while(true) {
        ssize_t bytes_read = recv(client, buffer, sizeof(buffer), 0);

        if(bytes_read<=0)
            break;
        
        pending.append(buffer, static_cast<size_t>(bytes_read));
        size_t pos;

        while((pos = pending.find('\n')) != string::npos) {
            if(pos > MAX_REQUEST_SIZE) {
                send_all(client, "ERROR\n");
                close(client);
                return;
            }

            string request = pending.substr(0, pos);
            pending.erase(0, pos+1);

            Command command = Parser::parse(request);

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

            if(!send_all(client, response))
                break;
        }

        if(pending.size() > MAX_REQUEST_SIZE) {
            send_all(client, "ERROR\n");
            break;
        }
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

        size_t current_clients = active_clients.fetch_add(1);

        if(current_clients>=MAX_CLIENTS) {
            active_clients.fetch_sub(1);
            close(client);
            continue;
        }

        timeval timeout{};
        timeout.tv_sec = 30;

        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

#ifdef SO_NOSIGPIPE
        int no_sigpipe = 1;
        setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &no_sigpipe, sizeof(timeout));
#endif

        thread_pool.enqueue([this, client]() {
            handle_client(client);
            active_clients.fetch_sub(1);
        });
    }
}