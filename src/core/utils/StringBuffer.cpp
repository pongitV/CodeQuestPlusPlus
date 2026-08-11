#include "StringBuffer.h"

StringBuffer::StringBuffer(size_t initialCapacity) {
    Reserve(initialCapacity);
}

void StringBuffer::Reserve(size_t size) {
    capacity = size;
    buffer.reserve(size);
}

void StringBuffer::Append(const std::string& text) {
    buffer.push_back(text);
}

void StringBuffer::Append(const char* text) {
    if (text) buffer.emplace_back(text);
}

void StringBuffer::Append(int number) {
    buffer.push_back(std::to_string(number));
}

void StringBuffer::Append(float number) {
    buffer.push_back(std::to_string(number));
}

std::vector<std::string> StringBuffer::ToVector() const {
    return buffer;
}

void StringBuffer::Clear() {
    buffer.clear();
}
