#include "NPCAppearance.h"
#include "NPCAppearanceLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/progress/Progression.h"
#include "../../../rendering/raycaster/engine-raycaster/RaycasterHUD.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCAppearance::interact(Character* player) {
    InputControl::executarLoopMenuPopup(
        [this, player]() { return this->getDialogue(player); },
        [this, player]() { return this->getMenuOptions(player, 120); },
        [this, player](const std::string& op) { this->processOption(player, op, 120); return true; },
        getPlaceName(), getHeaderColor(), getASCIIArt()
    );
}

std::string NPCAppearance::getPlaceName() const {
    return "SALA DE CUSTOMIZACAO";
}

Color NPCAppearance::getHeaderColor() const {
    return Color::YELLOW_CLARO;
}

Color NPCAppearance::getArtColor() const {
    return Color::YELLOW_CLARO;
}

const std::vector<std::string>& NPCAppearance::getASCIIArt() const {
    return NPCAparenciaLayouts::arteAparencia;
}

std::vector<std::string> NPCAppearance::getDialogue(Character* /*jogador*/) {
    std::vector<std::string> lines = {
        "Saudacoes, viajante! Eu sou Anok.",
        "Deseja renovar seu estilo?",
        "Aqui voce pode comprar novos icones de exibicao para o map",
        "e novas cores para o HUD de tijolos para deixar sua jornada unica!"
    };
    return lines;
}

std::vector<std::string> NPCAppearance::getMenuOptions(Character* /*jogador*/, int /*larguraTerminal*/) {
    return {
        "Comprar Icones",
        "Comprar Cores de Fundo",
        "Mudar Appearance Atual",
        "Voltar"
    };
}

void NPCAppearance::processOption(Character* player, const std::string& option, int /*larguraTerminal*/) {
    auto& progress = Progression::instance();

    if (option == "Comprar Icones") {
        std::vector<std::pair<std::string, std::pair<char, int>>> storeIcons = {
            {"Coracao (â™¥)", {'H', 100}}, // Usando caractere comum H ou simbolo se o terminal suportar. Para seguranca de UTF-8, usamos caracteres visiveis elegantes
            {"Estrela (*)", {'S', 150}},
            {"Espadas (X)", {'X', 200}},
            {"Coroa (K)", {'K', 400}},
            {"Cifrao ($)", {'$', 300}}
        };

        std::vector<std::string> itemOptions;
        std::vector<std::pair<std::string, std::pair<char, int>>> available;

        for (const auto& item : storeIcons) {
            std::string flag = "Aparencia_Icone_" + std::string(1, item.second.first);
            if (!progress.getFlag(flag)) {
                itemOptions.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                available.push_back(item);
            }
        }

        if (itemOptions.empty()) {
            return;
        }

        itemOptions.push_back("Voltar");

        int choice = InputControl::lerSelecaoMenuEmPopup(
            "COMPRAR ICONES",
            {"Seu Ouro: " + std::to_string(player->getInventory()->getGold()) + "G", "Selecione um icone para comprar:"},
            itemOptions, Color::YELLOW_CLARO, getASCIIArt()
        );

        if (choice >= 0 && choice < (int)available.size()) {
            auto selected = available[choice];
            if (player->getInventory()->getGold() >= selected.second.second) {
                player->getInventory()->addGold(-selected.second.second);
                std::string flag = "Aparencia_Icone_" + std::string(1, selected.second.first);
                progress.setFlag(flag, true);
                
            } else {
            }
        }
    }
    else if (option == "Comprar Cores de Fundo") {
        std::vector<std::pair<std::string, std::pair<Color, int>>> storeColors = {
            {"Fundo Azul", {Color::FUNDO_BLUE, 150}},
            {"Fundo Verde", {Color::FUNDO_GREEN, 150}},
            {"Fundo Vermelho", {Color::FUNDO_RED, 200}},
            {"Fundo Magenta", {Color::FUNDO_MAGENTA, 250}},
            {"Fundo Ciano", {Color::FUNDO_CYAN, 250}}
        };

        std::vector<std::string> itemOptions;
        std::vector<std::pair<std::string, std::pair<Color, int>>> available;

        for (const auto& item : storeColors) {
            std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second.first));
            if (!progress.getFlag(flag)) {
                itemOptions.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                available.push_back(item);
            }
        }

        if (itemOptions.empty()) {
            return;
        }

        itemOptions.push_back("Voltar");

        int choice = InputControl::lerSelecaoMenuEmPopup(
            "COMPRAR CORES DE FUNDO",
            {"Seu Ouro: " + std::to_string(player->getInventory()->getGold()) + "G", "Selecione uma cor para comprar:"},
            itemOptions, Color::YELLOW_CLARO, getASCIIArt()
        );

        if (choice >= 0 && choice < (int)available.size()) {
            auto selected = available[choice];
            if (player->getInventory()->getGold() >= selected.second.second) {
                player->getInventory()->addGold(-selected.second.second);
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(selected.second.first));
                progress.setFlag(flag, true);
                
            } else {
            }
        }
    }
    else if (option == "Mudar Appearance Atual") {
        std::vector<std::string> subOptions = {"Mudar Icone do Jogador", "Mudar Cor do HUD de Tijolos", "Voltar"};
        int subChoice = InputControl::lerSelecaoMenuEmPopup(
            "MUDAR APARENCIA",
            {"Escolha o que deseja customizar:"},
            subOptions, Color::YELLOW_CLARO, getASCIIArt()
        );

        if (subChoice == 0) {
            // Icone
            std::vector<std::pair<std::string, char>> availableIcons = {
                {"Icone Padrao (@)", '@'}
            };

            std::vector<std::pair<std::string, char>> storeIcons = {
                {"Coracao (â™¥)", 'H'},
                {"Estrela (*)", 'S'},
                {"Espadas (X)", 'X'},
                {"Coroa (K)", 'K'},
                {"Cifrao ($)", '$'}
            };

            for (const auto& item : storeIcons) {
                std::string flag = "Aparencia_Icone_" + std::string(1, item.second);
                if (progress.getFlag(flag)) {
                    availableIcons.push_back(item);
                }
            }

            std::vector<std::string> menuOptions;
            for (const auto& ic : availableIcons) {
                std::string status = ('@' == ic.second) ? " (Equipado)" : "";
                menuOptions.push_back(ic.first + status);
            }
            menuOptions.push_back("Voltar");

            int choice = InputControl::lerSelecaoMenuEmPopup(
                "SELECIONAR ICONE",
                {"Selecione o icone de exibicao no map:"},
                menuOptions, Color::YELLOW_CLARO, getASCIIArt()
            );

            if (choice >= 0 && choice < (int)availableIcons.size()) {
                // Appearance::iconeAtivo = availableIcons[choice].second;
            }
        }
        else if (subChoice == 1) {
            // Cor de fundo
            std::vector<std::pair<std::string, Color>> availableColors = {
                {"Fundo Padrao (Preto)", Color::RESET}
            };

            std::vector<std::pair<std::string, Color>> storeColors = {
                {"Fundo Azul", Color::FUNDO_BLUE},
                {"Fundo Verde", Color::FUNDO_GREEN},
                {"Fundo Vermelho", Color::FUNDO_RED},
                {"Fundo Magenta", Color::FUNDO_MAGENTA},
                {"Fundo Ciano", Color::FUNDO_CYAN}
            };

            for (const auto& item : storeColors) {
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second));
                if (progress.getFlag(flag)) {
                    availableColors.push_back(item);
                }
            }

            std::vector<std::string> menuOptions;
            for (const auto& c : availableColors) {
                std::string status = (Color::FUNDO_BLACK == c.second) ? " (Equipado)" : "";
                menuOptions.push_back(c.first + status);
            }
            menuOptions.push_back("Voltar");

            int choice = InputControl::lerSelecaoMenuEmPopup(
                "SELECIONAR COR DO HUD",
                {"Selecione a cor do HUD de tijolos:"},
                menuOptions, Color::YELLOW_CLARO, getASCIIArt()
            );

            if (choice >= 0 && choice < (int)availableColors.size()) {
                g_hudBrickColor = availableColors[choice].second;
            }
        }
    }
}
