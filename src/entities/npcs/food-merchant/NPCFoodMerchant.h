#pragma once

#include "../../character/Character.h"
#include <string>
#include <vector>
#include "../NPCInteraction.h"
#include "../../../core/utils/Color.h"

class NPCFoodMerchant : public NPCInteraction
{
public:
    void interagir(Character* jogador);
    void interact(Character* player) { interagir(player); }

protected:
    std::string getNameDoLugar() const override;
    Color obterCorDoCabecalho() const override;
    Color obterCorDaArte() const override;
    const std::vector<std::string>& obterArteASCII() const override;

    std::vector<std::string> obterDialogo(Character* jogador) override;
    std::vector<std::string> obterOpcoesMenu(Character* jogador, int larguraDoTerminal) override;
    void processarOpcao(Character* jogador, const std::string& opcao, int larguraDoTerminal) override;
};

using NPCFood = NPCFoodMerchant;
