#include "ScreenBestiaryRaycaster.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../core/window/GameWindow.h"
#include "../../../../systems/progress/Bestiary.h"
#include "../utils/MenuD2DUtils.h"
#include "../../../../entities/character/Character.h"
#include "../../../../entities/races/RaceBase.h"
#include <vector>
#include <string>
#include <algorithm>

void TelaBestiarioRaycaster::displayDetalhe(Character* enemy) {
    if (!enemy) return;
    std::string nomeStr = enemy->getName();
    const auto* info = Bestiary::instancia().obterInfo(nomeStr);
    
    std::vector<std::string> textoContexto;
    textoContexto.push_back("Raça / Classe: " + enemy->obterRaca()->getRaceName() + " " + enemy->getNameClasse());
    textoContexto.push_back("Nível: " + std::to_string(enemy->getLevel()) + "  |  HP Max: " + std::to_string(enemy->obterVidaMaxima()));
    textoContexto.push_back("Força: " + std::to_string(enemy->getStrength()) + "  |  Destreza: " + std::to_string(enemy->getDexterity()) + "  |  Inteligência: " + std::to_string(enemy->getInteligencia()));
    
    if (info) {
        if (!info->habitat.empty()) textoContexto.push_back("Habitat: " + info->habitat);
        if (!info->lore.empty()) textoContexto.push_back("Lore: " + info->lore);
    }
    
    int derrotas = Bestiary::instancia().obterQuantidadeDerrotas(nomeStr);
    textoContexto.push_back("Total de Derrotas: " + std::to_string(derrotas));

    std::vector<std::string> arte = (info && !info->appearance.empty()) ? info->appearance : std::vector<std::string>{};
    static const std::vector<GrupoCorUI> paletaDinamica = {
        {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-=_+[]{}|;':\",./<>?\\~` ", 220, 100, 40}
    };

    MenuRaycasterUtils::renderizarMenuPadrao(
        MenuRaycasterUtils::utf8_to_wstring("BESTIARIO - " + nomeStr),
        D2D1::ColorF(0.9f, 0.4f, 0.2f),
        { "VOLTAR" },
        arte,
        paletaDinamica,
        arte.empty() ? MenuRaycasterUtils::PosicaoArte::NENHUMA : MenuRaycasterUtils::PosicaoArte::ESQUERDA,
        4.0f,
        nullptr, nullptr, {}, {},
        textoContexto
    );
}

void TelaBestiarioRaycaster::display(const std::vector<Character*>& enemies) {
    InputControl::limparBuffer();
    
    auto ordemInimigos = Bestiary::instancia().obterInimigosOrdenadosPorDificuldade();
    if (ordemInimigos.empty() && !enemies.empty()) {
        for (auto* e : enemies) {
            if (e) ordemInimigos.push_back(e->getName());
        }
    }
    
    if (ordemInimigos.empty()) {
        ordemInimigos = { "Goblin", "Slime", "Troll", "Fairy", "Mimic" };
    }

    while (true) {
        std::vector<std::string> opcoesMenu;
        std::vector<std::string> nomesInimigosMap;

        for (const auto& nomeIni : ordemInimigos) {
            bool descoberto = Bestiary::instancia().estaDescoberto(nomeIni);
            int derrotas = Bestiary::instancia().obterQuantidadeDerrotas(nomeIni);
            const auto* info = Bestiary::instancia().obterInfo(nomeIni);
            
            if (descoberto || info != nullptr) {
                std::string infoLabel = nomeIni;
                if (derrotas > 0) infoLabel += " (Derrotas: " + std::to_string(derrotas) + ")";
                else if (descoberto) infoLabel += " (Descoberto)";
                opcoesMenu.push_back(infoLabel);
                nomesInimigosMap.push_back(nomeIni);
            } else {
                opcoesMenu.push_back("??? (Desconhecido)");
                nomesInimigosMap.push_back("");
            }
        }
        opcoesMenu.push_back("VOLTAR");

        int escolha = MenuRaycasterUtils::renderizarMenuPadrao(
            L"BESTIÁRIO DE CRIATURAS",
            D2D1::ColorF(0.9f, 0.4f, 0.2f),
            opcoesMenu,
            {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
            nullptr, nullptr, {}, {},
            { "Selecione uma criatura para consultar detalhes de combate:" }
        );

        if (escolha < 0 || escolha == (int)opcoesMenu.size() - 1) {
            break;
        }

        if (escolha >= 0 && escolha < (int)nomesInimigosMap.size()) {
            std::string nomeAlvo = nomesInimigosMap[escolha];
            if (nomeAlvo.empty()) {
                InputControl::limparBuffer();
                InputControl::aguardarEnter("Você ainda não descobriu esta criatura!");
            } else {
                const auto* info = Bestiary::instancia().obterInfo(nomeAlvo);
                std::vector<std::string> textoContexto;
                int derrotas = Bestiary::instancia().obterQuantidadeDerrotas(nomeAlvo);
                textoContexto.push_back("Total de Derrotas: " + std::to_string(derrotas));

                if (info) {
                    textoContexto.push_back("Nome: " + info->nome);
                    if (!info->habitat.empty()) textoContexto.push_back("Habitat: " + info->habitat);
                    if (!info->lore.empty()) textoContexto.push_back("Lore: " + info->lore);
                    if (!info->funFact.empty()) textoContexto.push_back("Curiosidade: " + info->funFact);
                    if (!info->habilidadePassiva.empty()) textoContexto.push_back("Passiva: " + info->habilidadePassiva);
                    for (const auto& atr : info->atributosTexto) textoContexto.push_back(" " + atr);
                } else {
                    textoContexto.push_back("Informações detalhadas indisponíveis.");
                }

                std::vector<std::string> arte = (info && !info->appearance.empty()) ? info->appearance : std::vector<std::string>{};
                static const std::vector<GrupoCorUI> paletaDinamica = {
                    {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-=_+[]{}|;':\",./<>?\\~` ", 220, 100, 40}
                };

                InputControl::limparBuffer();
                MenuRaycasterUtils::renderizarMenuPadrao(
                    MenuRaycasterUtils::utf8_to_wstring("FICHA DA CRIATURA - " + nomeAlvo),
                    D2D1::ColorF(0.9f, 0.4f, 0.2f),
                    { "VOLTAR" },
                    arte,
                    paletaDinamica,
                    arte.empty() ? MenuRaycasterUtils::PosicaoArte::NENHUMA : MenuRaycasterUtils::PosicaoArte::ESQUERDA,
                    3.5f,
                    nullptr, nullptr, {}, {},
                    textoContexto
                );
                InputControl::limparBuffer();
            }
        }
    }
}
