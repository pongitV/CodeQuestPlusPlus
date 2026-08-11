#pragma once

#include "../../character/Character.h"
#include <string>
#include <vector>
#include "../NPCInteraction.h"
#include "../../../core/utils/Color.h"

class NPCMageNPC : public NPCInteraction
{
public:
    void interagir(Character* jogador);

protected:
    // INFORMACOES DO LUGAR E APARENCIA
    std::string getNameDoLugar() const override;
    Color obterCorDoCabecalho() const override;
    Color obterCorDaArte() const override;
    const std::vector<std::string>& obterArteASCII() const override;

    // INTERACAO E MENU
    std::vector<std::string> obterDialogo(Character* jogador) override;
    std::vector<std::string> obterOpcoesMenu(Character* jogador, int larguraDoTerminal) override;
    void processarOpcao(Character* jogador, const std::string& opcao, int larguraDoTerminal) override;
};
