#include "ScreenTutorial.h"
#include "ScreenMenu.h"
#include <vector>
#include <string>

#include "../../../core/utils/InputControl.h"
#include "../../../core/utils/RandomGenerator.h"
#include "../../../systems/combat/Parry.h"
#include "../../../rendering/raycaster/screens/utils/MenuD2DUtils.h"

static void rodarTutorialMovimentoD2D() {
    std::vector<std::string> explicacao = {
        "--- TUTORIAL DE PARRY MOVIMENTO ---",
        "Uma barra com um marcador deslizante aparecerá na tela.",
        "Pressione a tecla ESPAÇO no exato momento em que o indicador",
        "estiver dentro da zona verde [ | ]!",
        "Um tempo perfeito garante a defesa e reflete o ataque."
    };

    MenuRaycasterUtils::renderizarMenuPadrao(
        L"PARRY MOVIMENTO - COMO FUNCIONA",
        D2D1::ColorF(0.2f, 0.8f, 0.4f),
        { "ENTENDIDO" },
        {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
        nullptr, nullptr, {}, {},
        explicacao
    );
}

static void rodarTutorialDigitacaoD2D() {
    std::vector<std::string> explicacao = {
        "--- TUTORIAL DE PARRY DIGITAÇÃO ---",
        "Uma sequência de números aparecerá na tela com tempo limite.",
        "Digite os números rapidamente na sequência exata e pressione ENTER.",
        "Se for ágil o suficiente, você bloqueará completamente o dano!"
    };

    MenuRaycasterUtils::renderizarMenuPadrao(
        L"PARRY DIGITAÇÃO - COMO FUNCIONA",
        D2D1::ColorF(0.8f, 0.4f, 0.2f),
        { "ENTENDIDO" },
        {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
        nullptr, nullptr, {}, {},
        explicacao
    );
}

void TelaTutorial::displayTutorialDeParry(const std::string& infoBox) {
    InputControl::limparBuffer();

    while (true) {
        std::vector<std::string> explicacao = {
            "Quando um inimigo atacar, você pode reagir com um Parry.",
            "Existem dois modos de Parry no jogo:",
            "  1. MOVIMENTO (Minigame de Barra deslizante com ESPAÇO)",
            "  2. DIGITAÇÃO (Minigame de Sequência Numérica com ENTER)",
            "",
            "Escolha o tutorial que deseja realizar:"
        };

        if (!infoBox.empty()) {
            explicacao.push_back("Nota: " + infoBox);
        }

        std::vector<std::string> opcoes = {
            "TUTORIAL DE PARRY MOVIMENTO",
            "TUTORIAL DE PARRY DIGITAÇÃO",
            "REALIZAR AMBOS OS TUTORIAIS",
            "PULAR TUTORIAL"
        };

        int escolha = MenuRaycasterUtils::renderizarMenuPadrao(
            L"TUTORIAL DE PARRY",
            D2D1::ColorF(0.2f, 0.6f, 0.9f),
            opcoes,
            {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
            nullptr, nullptr, {}, {},
            explicacao
        );

        if (escolha == 0) {
            rodarTutorialMovimentoD2D();
            break;
        } else if (escolha == 1) {
            rodarTutorialDigitacaoD2D();
            break;
        } else if (escolha == 2) {
            rodarTutorialMovimentoD2D();
            rodarTutorialDigitacaoD2D();
            break;
        } else {
            break;
        }
    }
}
