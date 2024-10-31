# Multi-Threaded Web Server
A simple multi-threaded web server in C++ that handles concurrent HTTP requests using threading. This server includes features such as dynamic content handling, basic request logging, and request parsing for headers and query parameters.

Features
1. <strong> Multithreading </strong>: Concurrently handles multiple client requests.
2. <strong>Dynamic Content Handling </strong>: Supports templating for generating dynamic HTML content.
3. <strong>Logging </strong>: Logs HTTP requests, responses, and errors to a log file.
4. <strong>Request Parsing </strong>: Parses headers and query parameters from incoming requests.
5. <strong>Basic HTTP Routing </strong>: Routes requests based on the URL path.

Prerequisites
```
C++17 or higher
Clang or GCC compiler
POSIX-compliant environment (Linux or macOS)
Sockets and Pthreads libraries (typically included with POSIX systems)
```
Project Structure
```
.
├── server.cpp       # Main server code, handles client connections and responses
├── logger.h         # Header file for logging functions
├── logger.cpp       # Log request and error messages to file
├── request_parser.h # Header for request parsing functions
├── request_parser.cpp # Implements request parsing logic
├── templates        # Folder containing HTML templates
└── README.md        # Project documentation
```
Setup and Installation

1. Clone the repository
```
git clone https://github.com/your-username/multithreaded-web-server.git
cd multithreaded-web-server
```
2. Compile the Code
To compile all .cpp files together:

```
clang++ -std=c++17 -o server server.cpp logger.cpp request_parser.cpp -pthread
```
Or, compile each file individually and link:

```
clang++ -std=c++17 -c server.cpp -o server.o
clang++ -std=c++17 -c logger.cpp -o logger.o
clang++ -std=c++17 -c request_parser.cpp -o request_parser.o
clang++ -o server server.o logger.o request_parser.o -pthread
```
3. Run the Server
Once compiled, you can start the server on the default port (e.g., 8080):

```
./server
```
Usage
Connecting to the Server: Open a web browser or use curl to send requests.
```
curl http://localhost:8080
```
Logging: The server logs all requests and errors to server.log.


Templates: HTML templates for dynamic content are stored in the templates folder. You can modify or add templates as needed.


Customization
- Port: To change the server port, modify the PORT constant in server.cpp.
- Template Rendering: Add more HTML files to the templates folder and update routing logic as needed.
- Thread Count in Logs: Customize logging in logger.cpp to include information about thread IDs. <br>
### Example Code Snippets <br>
Logging a Request 
In logger.cpp: <br>

```
void log_request(const std::string &message) {
    std::ofstream log_file("server.log", std::ios::app);
    if (log_file.is_open()) {
        log_file << message << std::endl;
        log_file.close();
    }
}
```
Simple Template Rendering
In server.cpp:

```
std::string render_template(const std::string &template_file, const std::unordered_map<std::string, std::string> &values) {
    std::ifstream file(template_file);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    for (const auto &pair : values) {
        std::string placeholder = "{{ " + pair.first + " }}";
        size_t pos = content.find(placeholder);
        if (pos != std::string::npos) {
            content.replace(pos, placeholder.size(), pair.second);
        }
    }
    return content;
}
```
Troubleshooting
Symbol Not Found for Architecture arm64: Ensure all .cpp files are compiled and linked.
setsockopt Failed: Verify that your environment supports the options used in the code.