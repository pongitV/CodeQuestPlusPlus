#pragma once

#include <memory>

class IAtributosUI;
class IBestiarioUI;

class UIAdapterFactory {
public:
    virtual ~UIAdapterFactory() = default;
    virtual std::unique_ptr<IAtributosUI> criarAtributosUI() = 0;
    virtual std::unique_ptr<IBestiarioUI> criarBestiarioUI() = 0;
};
