#pragma once

#include <atomic>
#include "../../ui/UIRenderer.h"

class RendererProvider {
public:
    static UIRenderer* get() { return instancia.load(std::memory_order_relaxed); }
    static void set(UIRenderer* r) { instancia.store(r, std::memory_order_relaxed); }
private:
    static std::atomic<UIRenderer*> instancia;
};
