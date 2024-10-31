#include "logger.h"
#include <fstream>

void log_request(const std::string &message) {
    std::ofstream log_file("server.log", std::ios::app);
    log_file << message << std::endl;
}
