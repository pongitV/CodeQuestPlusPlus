#pragma once
#include <string>
#include <vector>
#include <sstream>

class StringConverter {
public:
    static std::vector<std::string> convertRawStringToArray(const std::string& input) {
        std::vector<std::string> lines;
        std::stringstream ss(input);
        std::string line;
        while (std::getline(ss, line, '\n')) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            lines.push_back(line);
        }
        // Remove an initial empty line if it was introduced by a raw string literal newline
        if (!lines.empty() && lines.front().empty()) {
            lines.erase(lines.begin());
        }
        return lines;
    }
};
