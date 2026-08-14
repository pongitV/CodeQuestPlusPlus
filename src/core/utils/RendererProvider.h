#pragma once

#include <atomic>
#include "../../ui/UIRenderer.h"

class RendererProvider {
public:
    static UIRenderer* get() { return instance.load(std::memory_order_relaxed); }
    static void set(UIRenderer* r) { instance.store(r, std::memory_order_relaxed); }
private:
    static std::atomic<UIRenderer*> instance;
};
