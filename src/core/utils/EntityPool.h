#pragma once

#include <vector>
#include <memory>
#include <algorithm>

template<typename T>
class EntityPool {
private:
    std::vector<std::unique_ptr<T>> freeObjects;
    std::vector<std::unique_ptr<T>> usedObjects;

public:
    EntityPool() = default;
    ~EntityPool() = default;

    template<typename... Args>
    T* acquire(Args&&... args) {
        if (!freeObjects.empty()) {
            auto obj = std::move(freeObjects.back());
            freeObjects.pop_back();
            usedObjects.push_back(std::move(obj));
            return usedObjects.back().get();
        }
        usedObjects.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return usedObjects.back().get();
    }

    void release(T* obj) {
        auto it = std::find_if(usedObjects.begin(), usedObjects.end(),
            [obj](const std::unique_ptr<T>& ptr) { return ptr.get() == obj; });
        if (it != usedObjects.end()) {
            freeObjects.push_back(std::move(*it));
            usedObjects.erase(it);
        }
    }

    void reset() {
        for (auto& obj : usedObjects) {
            freeObjects.push_back(std::move(obj));
        }
        usedObjects.clear();
    }

    size_t getFreeCount() const { return freeObjects.size(); }
    size_t getUsedCount() const { return usedObjects.size(); }
};
