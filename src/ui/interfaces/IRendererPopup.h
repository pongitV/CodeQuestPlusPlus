#pragma once

#include <vector>
#include <string>
#include "../../core/utils/Color.h"
class IRenderizadorPopup {
public:
    virtual ~IRenderizadorPopup() = default;
    virtual void displayPopup(const std::string& titulo, const std::vector<std::string>& texto, Color corTema = Color::WHITE, const std::vector<std::string>& arteAscii = {}) = 0;
    virtual void iniciarInteracaoPopup() = 0;
    virtual int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Color corTema = Color::WHITE, const std::vector<std::string>& arteLogo = {}, bool voltarHabilitado = true) = 0;
};
