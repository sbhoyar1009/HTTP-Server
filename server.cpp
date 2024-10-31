#include "server.h"
#include "request.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>

constexpr int PORT = 8080;

void handle_client(int client_socket) {
    process_request(client_socket);
}

void start_server() {
    int server_socket;
    struct sockaddr_in address;

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return;
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        return;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        int client_socket;
        if ((client_socket = accept(server_socket, nullptr, nullptr)) < 0) {
            perror("Accept failed");
            continue;
        }
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach(); // Detach the thread to handle multiple clients concurrently
    }
}
