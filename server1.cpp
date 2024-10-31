#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <thread>
#include <vector>
#include <fstream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <atomic> // Add this include for atomic operations

std::atomic<int> active_thread_count{0}; // Global atomic counter for active threads

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 4096;

// Function to log requests
void log_request(const std::string &message) {
    std::ofstream log_file("server.log", std::ios::app);
    log_file << message << std::endl;
    if (log_file.is_open()) {
        log_file << "[Thread Count: " << active_thread_count.load() << "] " << message << std::endl;
        log_file.close();
    }
}


// Function to parse query parameters
std::unordered_map<std::string, std::string> parse_query_params(const std::string &query) {
    std::unordered_map<std::string, std::string> params;
    std::istringstream query_stream(query);
    std::string key_value;
    
    while (std::getline(query_stream, key_value, '&')) {
        size_t pos = key_value.find('=');
        if (pos != std::string::npos) {
            std::string key = key_value.substr(0, pos);
            std::string value = key_value.substr(pos + 1);
            params[key] = value;
        }
    }
    
    return params;
}

// Function to render the HTML template
std::string render_template(const std::string &template_file, const std::unordered_map<std::string, std::string> &values) {
    std::ifstream file(template_file);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    for (const auto &pair : values) {
        std::string placeholder = "{{" + pair.first + "}}";
        size_t pos = content.find(placeholder);
        while (pos != std::string::npos) {
            content.replace(pos, placeholder.length(), pair.second);
            pos = content.find(placeholder, pos + pair.second.length());
        }
    }
    
    return content;
}

// Function to handle client connections
void handle_client(int client_socket) {
    active_thread_count++;  // Increment thread count
    log_request("New connection. Active threads: " + std::to_string(active_thread_count.load()));
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, sizeof(buffer));
    
    // Read the client's request
    read(client_socket, buffer, sizeof(buffer) - 1);
    
    std::string request(buffer);
    log_request("Received: " + request); // Log request
    
    std::string response;
    std::string method, path, version, query_string;
    std::unordered_map<std::string, std::string> headers;
    
    // Simple request parsing
    std::istringstream request_stream(request);
    request_stream >> method >> path >> version;
    
    // Check for query parameters
    size_t pos = path.find('?');
    if (pos != std::string::npos) {
        query_string = path.substr(pos + 1);
        path = path.substr(0, pos);
    }
    
    // Parse headers
    std::string line;
    while (std::getline(request_stream, line) && line != "\r") {
        size_t separator_pos = line.find(": ");
        if (separator_pos != std::string::npos) {
            std::string header_name = line.substr(0, separator_pos);
            std::string header_value = line.substr(separator_pos + 2, line.length() - separator_pos - 3);
            headers[header_name] = header_value;
        }
    }
    
    // Validate request method
    if (method != "GET") {
        response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        write(client_socket, response.c_str(), response.length());
        log_request("405 Method Not Allowed for request: " + request);
        close(client_socket);
        return;
    }

    // Handle requests
    if (path == "/dynamic") {
        std::unordered_map<std::string, std::string> values;
        values["message"] = "Hello, Dynamic World!";
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        response += render_template("template.html", values);
    } else {
        response = "HTTP/1.1 404 Not Found\r\n\r\n";
        log_request("404 Not Found for request: " + request);
    }
    
    // Send response
    write(client_socket, response.c_str(), response.length());
    log_request("Response sent: " + response);
    close(client_socket);
        active_thread_count--;  // Decrement thread count
    log_request("Connection closed. Active threads: " + std::to_string(active_thread_count.load()));
}

// Main function to set up the server
int main() {
    int server_socket;
    struct sockaddr_in address;
    
    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return EXIT_FAILURE;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return EXIT_FAILURE;
    }
    
    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        return EXIT_FAILURE;
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

    return 0;
}
