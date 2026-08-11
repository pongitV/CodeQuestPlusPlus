#include "NPCPriest.h"
#include "NPCPriestLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCPriest::interagir(Character* jogador) {
    
    std::vector<std::string> linhas = {
        "Que a paz e a luz dos deuses guiem seus passos, meu filho.",
        "Nesta sagrada igreja do reino, oferecemos refugio e cura para as almas fatigadas.",
        "Sinto uma aura extremamente sombria emanando do palacio real no norte...",
        "Prepare-se bem antes de desafiar o que quer que resida la."
    };

    while (true) {
        std::vector<std::string> opcoes = obterOpcoesMenu(jogador, 80);
        int escolha = InputControl::lerSelecaoMenuEmPopup("PADRE BENEDITO", {"O que deseja fazer?"}, opcoes, Color::CYAN);
        
        if (escolha >= 0 && escolha < (int)opcoes.size()) {
            std::string opcao = opcoes[escolha];
            if (opcao == "Voltar") {
                break;
            }
            processarOpcao(jogador, opcao, 80);
        } else {
            break;
        }
    }
}

std::string NPCPriest::getNameDoLugar() const {
    return "ALTAR DA IGREJA";
}

Color NPCPriest::obterCorDoCabecalho() const {
    return Color::CYAN;
}

Color NPCPriest::obterCorDaArte() const {
    return Color::CYAN;
}

const std::vector<std::string>& NPCPriest::obterArteASCII() const {
    return NPCPriestLayouts::artePriest;
}

std::vector<std::string> NPCPriest::obterDialogo(Character* /*jogador*/) {
    return { "Que os Deuses iluminem seu caminho, filho." };
}

std::vector<std::string> NPCPriest::obterOpcoesMenu(Character* jogador, int /*larguraDoTerminal*/) {
    return {
        "Pedir Bencao (Restaurar HP)",
        "Conversar sobre o Palacio",
        "Voltar"
    };
}

void NPCPriest::processarOpcao(Character* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    if (opcao == "Pedir Bencao (Restaurar HP)") {
        if (jogador->obterVida() >= jogador->obterVidaMaxima()) {
        } else {
            jogador->modificarVida(jogador->obterVidaMaxima());
        }
    }
    else if (opcao == "Conversar sobre o Palacio") {
        std::vector<std::string> lore = {
            "O palácio real costumava ser o farol de esperança do Kingdom.",
            "Contudo, há algumas semanas, o rei trancou-se em seus aposentos",
            "e ordenou que guardas mágicos selassem a entrada.",
            "Ninguém entra ou sai. Barulhos aterrorizantes são ouvidos à noite.",
            "Temo que o pior tenha acontecido com a realeza..."
        };
    }
}
