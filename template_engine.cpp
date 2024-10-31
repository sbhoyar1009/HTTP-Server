#include "template_engine.h"
#include <fstream>
#include <sstream>

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
