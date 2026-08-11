#pragma once

#include <string>
#include <unordered_map>
#include <variant>

class Config {
private:
    std::unordered_map<std::string, std::variant<int, float, bool, std::string>> settings;

public:
    Config() = default;
    ~Config() = default;

    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const {
        auto it = settings.find(key);
        if (it == settings.end()) {
            return defaultValue;
        }
        if (auto val = std::get_if<T>(&it->second)) {
            return *val;
        }
        return defaultValue;
    }

    template<typename T>
    void set(const std::string& key, const T& value) {
        settings[key] = value;
    }

    bool has(const std::string& key) const {
        return settings.find(key) != settings.end();
    }
};
