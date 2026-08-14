#include "NPCPriest.h"
#include "NPCPriestLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCPriest::interact(Character* player) {
    std::vector<std::string> lines = {
        "Que a paz e a luz dos deuses guiem seus passos, meu filho.",
        "Nesta sagrada igreja do reino, oferecemos refugio e cura para as almas fatigadas.",
        "Sinto uma aura extremamente sombria emanando do palacio real no norte...",
        "Prepare-se bem antes de desafiar o que quer que resida la."
    };

    while (true) {
        std::vector<std::string> options = getMenuOptions(player, 80);
        int choice = InputControl::lerSelecaoMenuEmPopup("PADRE BENEDITO", {"O que deseja fazer?"}, options, Color::CYAN);
        
        if (choice >= 0 && choice < (int)options.size()) {
            std::string option = options[choice];
            if (option == "Voltar") {
                break;
            }
            processOption(player, option, 80);
        } else {
            break;
        }
    }
}

std::string NPCPriest::getPlaceName() const {
    return "ALTAR DA IGREJA";
}

Color NPCPriest::getHeaderColor() const {
    return Color::CYAN;
}

Color NPCPriest::getArtColor() const {
    return Color::CYAN;
}

const std::vector<std::string>& NPCPriest::getASCIIArt() const {
    return NPCPriestLayouts::priestArt;
}

std::vector<std::string> NPCPriest::getDialogue(Character* /*player*/) {
    return { "Que os Deuses iluminem seu caminho, filho." };
}

std::vector<std::string> NPCPriest::getMenuOptions(Character* /*player*/, int /*terminalWidth*/) {
    return {
        "Pedir Bencao (Restaurar HP)",
        "Conversar sobre o Palacio",
        "Voltar"
    };
}

void NPCPriest::processOption(Character* player, const std::string& option, int /*terminalWidth*/) {
    if (option == "Pedir Bencao (Restaurar HP)") {
        if (player->getHealth() >= player->getMaxHealth()) {
        } else {
            player->modifyHealth(player->getMaxHealth());
        }
    }
    else if (option == "Conversar sobre o Palacio") {
        std::vector<std::string> lore = {
            "O palácio real costumava ser o farol de esperança do Kingdom.",
            "Contudo, há algumas semanas, o rei trancou-se em seus aposentos",
            "e ordenou que guardas mágicos selassem a entrada.",
            "Ninguém entra ou sai. Barulhos aterrorizantes são ouvidos à noite.",
            "Temo que o pior tenha acontecido com a realeza..."
        };
    }
}
