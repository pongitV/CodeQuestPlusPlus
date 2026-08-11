#pragma once

#include <string>
#include "../../core/utils/Color.h"
class IRenderizadorTela {
public:
    virtual ~IRenderizadorTela() = default;
    virtual void limparTela() = 0;
    virtual void displayPainelTexto(const std::string& texto, Color cor) = 0;
};
