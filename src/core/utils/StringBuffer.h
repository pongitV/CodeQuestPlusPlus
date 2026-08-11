#pragma once

#include <string>
#include <vector>

// Buffer de strings reutilizável para evitar alocações dinâmicas em loops de renderização/UI
class StringBuffer {
private:
    std::vector<std::string> buffer;
    std::string current;
    size_t capacity = 0;

public:
    StringBuffer() = default;
    explicit StringBuffer(size_t initialCapacity);

    void Reserve(size_t size);
    void Append(const std::string& text);
    void Append(const char* text);
    void Append(int number);
    void Append(float number);
    const std::vector<std::string>& GetBuffer() const { return buffer; }
    std::vector<std::string> ToVector() const;
    void Clear();
    size_t Size() const { return buffer.size(); }
};
