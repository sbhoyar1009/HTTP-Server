#include "request.h"
#include "logger.h"
#include "template_engine.h"
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <cstring>
#include <unistd.h>

void process_request(int client_socket) {
    char buffer[4096];
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
}
