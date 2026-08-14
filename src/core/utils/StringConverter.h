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
        // Remove uma linha vazia inicial caso tenha sido introduzida por quebra de linha de raw string literal
        if (!lines.empty() && lines.front().empty()) {
            lines.erase(lines.begin());
        }
        return lines;
    }
};
