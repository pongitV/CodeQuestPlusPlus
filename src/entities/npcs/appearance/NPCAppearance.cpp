#include "NPCAppearance.h"
#include "NPCAppearanceLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/progress/Progression.h"
#include "../../../rendering/raycaster/engine-raycaster/RaycasterHUD.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCAparencia::interagir(Character* jogador) {
    InputControl::executarLoopMenuPopup(
        [this, jogador]() { return this->obterDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, 120); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, 120); return true; },
        getNameDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

std::string NPCAparencia::getNameDoLugar() const {
    return "SALA DE CUSTOMIZACAO";
}

Color NPCAparencia::obterCorDoCabecalho() const {
    return Color::YELLOW_CLARO;
}

Color NPCAparencia::obterCorDaArte() const {
    return Color::YELLOW_CLARO;
}

const std::vector<std::string>& NPCAparencia::obterArteASCII() const {
    return NPCAparenciaLayouts::arteAparencia;
}

std::vector<std::string> NPCAparencia::obterDialogo(Character* /*jogador*/) {
    std::vector<std::string> linhas = {
        "Saudacoes, viajante! Eu sou Anok.",
        "Deseja renovar seu estilo?",
        "Aqui voce pode comprar novos icones de exibicao para o map",
        "e novas cores para o HUD de tijolos para deixar sua jornada unica!"
    };
    return linhas;
}

std::vector<std::string> NPCAparencia::obterOpcoesMenu(Character* jogador, int /*larguraDoTerminal*/) {
    return {
        "Comprar Icones",
        "Comprar Cores de Fundo",
        "Mudar Appearance Atual",
        "Voltar"
    };
}

void NPCAparencia::processarOpcao(Character* jogador, const std::string& opcao, int larguraDoTerminal) {
    auto& progress = Progression::instancia();

    if (opcao == "Comprar Icones") {
        std::vector<std::pair<std::string, std::pair<char, int>>> iconesStore = {
            {"Coracao (â™¥)", {'H', 100}}, // Usando caractere comum H ou simbolo se o terminal suportar. Para seguranca de UTF-8, usamos caracteres visiveis elegantes
            {"Estrela (*)", {'S', 150}},
            {"Espadas (X)", {'X', 200}},
            {"Coroa (K)", {'K', 400}},
            {"Cifrao ($)", {'$', 300}}
        };

        std::vector<std::string> opcoesItem;
        std::vector<std::pair<std::string, std::pair<char, int>>> disponiveis;

        for (const auto& item : iconesStore) {
            std::string flag = "Aparencia_Icone_" + std::string(1, item.second.first);
            if (!progress.obterFlag(flag)) {
                opcoesItem.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                disponiveis.push_back(item);
            }
        }

        if (opcoesItem.empty()) {
            return;
        }

        opcoesItem.push_back("Voltar");

        int escolha = InputControl::lerSelecaoMenuEmPopup(
            "COMPRAR ICONES",
            {"Seu Ouro: " + std::to_string(jogador->obterInventario()->obterOuro()) + "G", "Selecione um icone para comprar:"},
            opcoesItem, Color::YELLOW_CLARO, obterArteASCII()
        );

        if (escolha >= 0 && escolha < (int)disponiveis.size()) {
            auto selecionado = disponiveis[escolha];
            if (jogador->obterInventario()->obterOuro() >= selecionado.second.second) {
                jogador->obterInventario()->adicionarOuro(-selecionado.second.second);
                std::string flag = "Aparencia_Icone_" + std::string(1, selecionado.second.first);
                progress.definirFlag(flag, true);
                
            } else {
            }
        }
    }
    else if (opcao == "Comprar Cores de Fundo") {
        std::vector<std::pair<std::string, std::pair<Color, int>>> coresStore = {
            {"Fundo Azul", {Color::FUNDO_BLUE, 150}},
            {"Fundo Verde", {Color::FUNDO_GREEN, 150}},
            {"Fundo Vermelho", {Color::FUNDO_RED, 200}},
            {"Fundo Magenta", {Color::FUNDO_MAGENTA, 250}},
            {"Fundo Ciano", {Color::FUNDO_CYAN, 250}}
        };

        std::vector<std::string> opcoesItem;
        std::vector<std::pair<std::string, std::pair<Color, int>>> disponiveis;

        for (const auto& item : coresStore) {
            std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second.first));
            if (!progress.obterFlag(flag)) {
                opcoesItem.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                disponiveis.push_back(item);
            }
        }

        if (opcoesItem.empty()) {
            return;
        }

        opcoesItem.push_back("Voltar");

        int escolha = InputControl::lerSelecaoMenuEmPopup(
            "COMPRAR CORES DE FUNDO",
            {"Seu Ouro: " + std::to_string(jogador->obterInventario()->obterOuro()) + "G", "Selecione uma cor para comprar:"},
            opcoesItem, Color::YELLOW_CLARO, obterArteASCII()
        );

        if (escolha >= 0 && escolha < (int)disponiveis.size()) {
            auto selecionado = disponiveis[escolha];
            if (jogador->obterInventario()->obterOuro() >= selecionado.second.second) {
                jogador->obterInventario()->adicionarOuro(-selecionado.second.second);
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(selecionado.second.first));
                progress.definirFlag(flag, true);
                
            } else {
            }
        }
    }
    else if (opcao == "Mudar Appearance Atual") {
        std::vector<std::string> subOpcoes = {"Mudar Icone do Jogador", "Mudar Cor do HUD de Tijolos", "Voltar"};
        int subEscolha = InputControl::lerSelecaoMenuEmPopup(
            "MUDAR APARENCIA",
            {"Escolha o que deseja customizar:"},
            subOpcoes, Color::YELLOW_CLARO, obterArteASCII()
        );

        if (subEscolha == 0) {
            // Icone
            std::vector<std::pair<std::string, char>> iconesDisponiveis = {
                {"Icone Padrao (@)", '@'}
            };

            std::vector<std::pair<std::string, char>> iconesStore = {
                {"Coracao (â™¥)", 'H'},
                {"Estrela (*)", 'S'},
                {"Espadas (X)", 'X'},
                {"Coroa (K)", 'K'},
                {"Cifrao ($)", '$'}
            };

            for (const auto& item : iconesStore) {
                std::string flag = "Aparencia_Icone_" + std::string(1, item.second);
                if (progress.obterFlag(flag)) {
                    iconesDisponiveis.push_back(item);
                }
            }

            std::vector<std::string> opcoesMenu;
            for (const auto& ic : iconesDisponiveis) {
                std::string status = ('@' == ic.second) ? " (Equipado)" : "";
                opcoesMenu.push_back(ic.first + status);
            }
            opcoesMenu.push_back("Voltar");

            int escolha = InputControl::lerSelecaoMenuEmPopup(
                "SELECIONAR ICONE",
                {"Selecione o icone de exibicao no map:"},
                opcoesMenu, Color::YELLOW_CLARO, obterArteASCII()
            );

            if (escolha >= 0 && escolha < (int)iconesDisponiveis.size()) {
                // Appearance::iconeAtivo = iconesDisponiveis[escolha].second;
            }
        }
        else if (subEscolha == 1) {
            // Color de Fundo
            std::vector<std::pair<std::string, Color>> coresDisponiveis = {
                {"Fundo Padrao (Preto)", Color::RESET}
            };

            std::vector<std::pair<std::string, Color>> coresStore = {
                {"Fundo Azul", Color::FUNDO_BLUE},
                {"Fundo Verde", Color::FUNDO_GREEN},
                {"Fundo Vermelho", Color::FUNDO_RED},
                {"Fundo Magenta", Color::FUNDO_MAGENTA},
                {"Fundo Ciano", Color::FUNDO_CYAN}
            };

            for (const auto& item : coresStore) {
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second));
                if (progress.obterFlag(flag)) {
                    coresDisponiveis.push_back(item);
                }
            }

            std::vector<std::string> opcoesMenu;
            for (const auto& c : coresDisponiveis) {
                std::string status = (Color::FUNDO_BLACK == c.second) ? " (Equipado)" : "";
                opcoesMenu.push_back(c.first + status);
            }
            opcoesMenu.push_back("Voltar");

            int escolha = InputControl::lerSelecaoMenuEmPopup(
                "SELECIONAR COR DO HUD",
                {"Selecione a cor do HUD de tijolos:"},
                opcoesMenu, Color::YELLOW_CLARO, obterArteASCII()
            );

            if (escolha >= 0 && escolha < (int)coresDisponiveis.size()) {
                g_hudBrickColor = coresDisponiveis[escolha].second;
            }
        }
    }
}

