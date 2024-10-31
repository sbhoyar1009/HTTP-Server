#ifndef TEMPLATE_ENGINE_H
#define TEMPLATE_ENGINE_H

#include <string>
#include <unordered_map>

std::string render_template(const std::string &template_file, const std::unordered_map<std::string, std::string> &values);

#endif // TEMPLATE_ENGINE_H
