#pragma once

#include <vector>
#include <string>
#include <cstring>

class StringArena {
private:
    std::vector<char> buffer;

public:
    StringArena(size_t initialCapacity = 2048) {
        buffer.reserve(initialCapacity);
    }
    ~StringArena() = default;

    std::string allocate(const std::string& str) {
        if (buffer.capacity() < buffer.size() + str.size() + 1) {
            buffer.reserve((buffer.capacity() + str.size() + 256) * 2);
        }
        size_t offset = buffer.size();
        buffer.insert(buffer.end(), str.begin(), str.end());
        buffer.push_back('\0');
        return std::string(&buffer[offset], str.size());
    }

    void clear() {
        buffer.clear();
    }
};
