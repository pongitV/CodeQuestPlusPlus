#pragma once

#include <string>
#include <vector>
#include "../../../../ui/screens/ScreenBase.h"
#include "../../../../ui/interfaces/IDiaryUI.h"
#include "../../../../core/utils/Color.h"

class Character;

class TelaDiarioRaycaster : public IDiarioUI {
public:
    void renderizarFundo() override;
    void displayCabecalho(int startY) override;
    void renderizarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, Color corCaixa, int minY, int startYOverride) override;
    void renderizarPopupMensagem(const std::string& titulo, const std::vector<std::string>& texto) override;
    void renderizarPopupInspecaoComArte(const std::string& titulo, const std::vector<std::string>& arte, const std::vector<std::string>& info, const std::string& subtitulo) override;
};
