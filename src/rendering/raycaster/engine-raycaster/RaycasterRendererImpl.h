#pragma once

#include "../../../ui/UIRenderer.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/utils/Color.h"

class RaycasterRendererImpl : public UIRenderer {
public:
    void displayPopup(const std::string& titulo, const std::vector<std::string>& falas, Color corCabecalho, const std::vector<std::string>& arteLogo = {}) override {
    }

    void iniciarInteracaoPopup() override {
    }

    int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& descricoes, const std::vector<std::string>& opcoes, Color corCabecalho, const std::vector<std::string>& arteLogo = {}, bool voltarHabilitado = true) override {
        return InputControl::lerSelecaoMenuEmPopup(titulo, descricoes, opcoes, corCabecalho, arteLogo, voltarHabilitado);
    }

    void limparTela() override { 
    }

    void displayPainelTexto(const std::string& texto, Color cor) override {
    }
};
