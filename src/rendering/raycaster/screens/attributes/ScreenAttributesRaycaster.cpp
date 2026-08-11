#include "ScreenAttributesRaycaster.h"
#include "../../../../ui/screens/attributes/ScreenAttributes.h"
#include "../../../../ui/screens/attributes/ScreenAttributesLayout.h"
#include "../../../../ui/screens/ScreenBase.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../systems/inventory/Item.h"
#include "../../../../entities/races/RaceBase.h"
#include "../../../../entities/classes/ClassBase.h"
#include "../../engine-raycaster/Raycaster.h"
#include "../utils/MenuD2DUtils.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "../../../../core/utils/Color.h"

struct EfeitoInfo {
    EfeitoID efeitoId;
    Color corId;
    const char* displayNome;
    bool mostrarTurnos;
};

static void displayTituloFlutuante(int startY) {
    int larguraConsole = 120;
    int logoHeight = ArtesAtributos::logoFicha.size();
    int logoY = startY - 1 - logoHeight;
    if (logoY < 0) logoY = 0;
    
    int compVisualLogo = 0;
    for (const auto& linha : ArtesAtributos::logoFicha) {
        int comp = (int)(linha).length();
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = (larguraConsole - compVisualLogo) / 2;
    if (logoX < 0) logoX = 0;
    
    std::string corTitulo = "";
    for (int i = 0; i < logoHeight; ++i) {
        const std::string& linha = ArtesAtributos::logoFicha[i];
        
        std::string buffer = linha;
    }
}

void TelaAtributosRaycaster::display(Character* currentPlayer) {
    gerenciarFichaDoJogador(currentPlayer);
}

enum EstadoAtributos { PRINCIPAL, HABILIDADES, DETALHES, SUBIR_NIVEL, ERRO_NIVEL };

void TelaAtributosRaycaster::gerenciarFichaDoJogador(Character* currentPlayer) {
    if (!currentPlayer) return;
    
    int selecaoAtual = 0;
    int selecaoSubir = 0;
    std::vector<std::string> opcoes = {"Subir de Nivel", "Habilidades e Equipamento", "Detalhes de Attributes", "Voltar"};
    EstadoAtributos state = PRINCIPAL;
    
    bool executando = true;
    
    while (executando) {
        double multiplicadorDeAtributosAtual = currentPlayer->obterMultiplicador();
        DebuffInfo debuff = TelaAtributos::calcularDebuff(currentPlayer);
        bool temBuff = debuff.temBuff;
        
        std::vector<std::string> currentOptions;
        int currentSelCount = 0;
        int* currentSelRef = nullptr;
        std::string tituloCaixa = "";
        
        if (state == PRINCIPAL) { 
            tituloCaixa = "FICHA DO JOGADOR";
            currentOptions = opcoes;
            currentSelCount = opcoes.size();
            currentSelRef = &selecaoAtual;
        } else if (state == HABILIDADES) { 
            tituloCaixa = "HABILIDADES & EQUIPAMENTOS"; 
            currentOptions = { "VOLTAR" };
            currentSelCount = 1;
        } else if (state == DETALHES) { 
            tituloCaixa = "DETALHES DE ATRIBUTOS"; 
            currentOptions = { "VOLTAR" };
            currentSelCount = 1;
        } else if (state == SUBIR_NIVEL) { 
            tituloCaixa = "SUBIR DE NIVEL"; 
            currentOptions = { "Vida (HP Maximo)", "Forca", "Destreza", "Resistencia", "Constituicao", "Inteligencia", "Sabedoria", "VOLTAR" };
            currentSelCount = 8;
            currentSelRef = &selecaoSubir;
        }
 else if (state == ERRO_NIVEL) { 
            tituloCaixa = "AVISO"; 
            currentOptions = { "VOLTAR" };
            currentSelCount = 1;
        }

        static const std::vector<GrupoCorUI> paletaAtributos = {
            {"█", 255, 0, 255},
            {"_", 255, 0, 255},
            {"|", 255, 0, 255},
            {"-", 255, 0, 255},
            {"/", 255, 0, 255},
            {"\\", 255, 0, 255}
        };

        auto renderCb = [&](UIDynamicBox& box, int selA, float optStartX, float optStartY, float pixelScale) {
            float lw = UIRenderer2D::LOGICAL_WIDTH;
            float lh = UIRenderer2D::LOGICAL_HEIGHT;
            float currentY = optStartY;
            
            float startX = 100.0f; // Left side for info
            float optionsX = startX + 650.0f; // Right side for options (side-by-side with info)
            
            if (state == PRINCIPAL) {
                // Linha 1: Nome, Raca, Classe
                box.AddText(L"NOME:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(MenuRaycasterUtils::utf8_to_wstring(currentPlayer->getName()), startX + 120, currentY, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f));
                currentY += 25.0f;
                
                box.AddText(L"RACA:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(MenuRaycasterUtils::utf8_to_wstring(currentPlayer->obterRaca()->getRaceName()), startX + 120, currentY, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f));
                currentY += 25.0f;
                
                box.AddText(L"CLASSE:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(MenuRaycasterUtils::utf8_to_wstring(currentPlayer->getNameClasse()), startX + 120, currentY, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f));
                currentY += 35.0f;
                
                // Linha 2: Nivel, HP, Ouro
                box.AddText(L"NIVEL:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(std::to_wstring(currentPlayer->getLevel()), startX + 120, currentY, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f));
                currentY += 25.0f;
                
                double pctVida = static_cast<double>(currentPlayer->obterVida()) / std::max(1, currentPlayer->obterVidaMaxima());
                D2D1_COLOR_F corVidaEnum = (pctVida > 0.7) ? D2D1::ColorF(0.2f, 1.0f, 0.2f) : ((pctVida > 0.3) ? D2D1::ColorF(1.0f, 1.0f, 0.2f) : D2D1::ColorF(1.0f, 0.2f, 0.2f));
                
                box.AddText(L"HP:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(std::to_wstring(currentPlayer->obterVida()) + L"/" + std::to_wstring(currentPlayer->obterVidaMaxima()), startX + 120, currentY, 16.0f, corVidaEnum);
                currentY += 25.0f;
                
                box.AddText(L"OURO:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(std::to_wstring(currentPlayer->obterInventario()->obterOuro()) + L"G", startX + 120, currentY, 16.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f));
                currentY += 35.0f;
                
                // Linha 3: Dificuldade e Parry
                std::wstring difStr = L"Normal";
                D2D1_COLOR_F difColor = D2D1::ColorF(1.0f, 1.0f, 0.2f);
                switch (currentPlayer->obterDificuldade()) {
                    case DificuldadeJogo::Facil: difStr = L"Facil"; difColor = D2D1::ColorF(0.2f, 1.0f, 0.2f); break;
                    case DificuldadeJogo::Normal: difStr = L"Normal"; difColor = D2D1::ColorF(1.0f, 1.0f, 0.2f); break;
                    case DificuldadeJogo::Dificil: difStr = L"Dificil"; difColor = D2D1::ColorF(1.0f, 0.2f, 0.2f); break;
                }
                box.AddText(L"DIFICULDADE:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(difStr, startX + 120, currentY, 16.0f, difColor);
                currentY += 25.0f;
                
                box.AddText(L"PARRY:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                if (currentPlayer->obterParryAtivado()) {
                    box.AddText(L"Ligado", startX + 120, currentY, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f));
                } else {
                    box.AddText(L"Desligado", startX + 120, currentY, 16.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f));
                }
                currentY += 35.0f;
                
                // Status
                box.AddText(L"STATUS:", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(L"Nenhum", startX + 120, currentY, 16.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f));
                currentY += 35.0f;
                
                // Atributos base
                float attrX = startX + 300.0f; // Secound column for Attributes
                float attrY = optStartY;
                
                auto drawAttr = [&](const std::wstring& name, int val, int debuffVal, float x, float y, D2D1_COLOR_F c) {
                    box.AddText(name + L":", x, y, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                    box.AddText(std::to_wstring(val), x + 150, y, 16.0f, c);
                    if (temBuff) {
                        box.AddText(L"(+" + std::to_wstring(static_cast<int>(val * (multiplicadorDeAtributosAtual - 1.0))) + L")", x + 180, y, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f));
                    }
                    if (debuffVal > 0) {
                        box.AddText(L"(-" + std::to_wstring(debuffVal) + L")", x + 230, y, 16.0f, D2D1::ColorF(1.0f, 0.2f, 0.2f));
                    }
                };
                
                drawAttr(L"Forca", currentPlayer->getStrength(), debuff.strengthPerdida, attrX, attrY, D2D1::ColorF(1.0f, 0.4f, 0.4f)); attrY += 25.0f;
                drawAttr(L"Destreza", currentPlayer->getDexterity(), debuff.dexterityPerdida, attrX, attrY, D2D1::ColorF(0.4f, 1.0f, 0.4f)); attrY += 25.0f;
                drawAttr(L"Resistencia", currentPlayer->getResistance(), debuff.resPerdida, attrX, attrY, D2D1::ColorF(1.0f, 1.0f, 0.4f)); attrY += 25.0f;
                drawAttr(L"Constituicao", currentPlayer->getConstitution(), debuff.constPerdida, attrX, attrY, D2D1::ColorF(0.4f, 1.0f, 1.0f)); attrY += 35.0f;
                
                // Poder de combate
                PoderCombate poder = TelaAtributos::calcularPoderCombate(currentPlayer, multiplicadorDeAtributosAtual);
                box.AddText(L"PODER DE COMBATE:", attrX, attrY, 16.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f));
                attrY += 25.0f;
                
                box.AddText(L"Dano Fisico:", attrX, attrY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(std::to_wstring(poder.danoFisEst), attrX + 120, attrY, 16.0f, D2D1::ColorF(1.0f, 0.2f, 0.2f));
                attrY += 25.0f;
                
                box.AddText(L"Dano Magico:", attrX, attrY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(std::to_wstring(poder.danoMagEst), attrX + 120, attrY, 16.0f, D2D1::ColorF(1.0f, 0.2f, 0.2f));
                attrY += 25.0f;
                
                box.AddText(L"Defesa Fixa:", attrX, attrY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(std::to_wstring(poder.defFixa), attrX + 120, attrY, 16.0f, D2D1::ColorF(0.2f, 0.4f, 1.0f));
                attrY += 25.0f;
                
                std::wostringstream ssMit; ssMit << std::fixed << std::setprecision(1) << poder.mitigacao;
                box.AddText(L"Mitigacao:", attrX, attrY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f));
                box.AddText(ssMit.str() + L"%", attrX + 120, attrY, 16.0f, D2D1::ColorF(0.0f, 0.8f, 1.0f));
                attrY += 25.0f;
                
                if (attrY > currentY) currentY = attrY;
            } 
            else if (state == HABILIDADES) {
                box.AddText(L"[HAB. Passiva de Race]  : " + MenuRaycasterUtils::utf8_to_wstring(currentPlayer->obterRaca()->getRaceAbilityName()), startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f));
                currentY += 25.0f;
                box.AddText(MenuRaycasterUtils::utf8_to_wstring(currentPlayer->obterRaca()->getRaceAbilityDescription()), startX + 20, currentY, 16.0f, D2D1::ColorF(0.7f, 0.7f, 0.7f));
                currentY += 40.0f;
                
                box.AddText(L"[HAB. Passiva de Class] : " + MenuRaycasterUtils::utf8_to_wstring(currentPlayer->obterClasse()->getNamePassivaClasse()), startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f));
                currentY += 25.0f;
                box.AddText(MenuRaycasterUtils::utf8_to_wstring(currentPlayer->obterClasse()->obterDescricaoPassivaClasse()), startX + 20, currentY, 16.0f, D2D1::ColorF(0.7f, 0.7f, 0.7f));
                currentY += 40.0f;
                
                box.AddText(L"[HAB. Ativa de Class]   : " + MenuRaycasterUtils::utf8_to_wstring(currentPlayer->obterClasse()->getNameHabilidadeClasse()), startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f));
                currentY += 25.0f;
                box.AddText(MenuRaycasterUtils::utf8_to_wstring(currentPlayer->obterClasse()->getClassAbilityDescription()), startX + 20, currentY, 16.0f, D2D1::ColorF(0.7f, 0.7f, 0.7f));
                currentY += 40.0f;
                
                box.AddText(L"EQUIPAMENTOS:", startX, currentY, 16.0f, D2D1::ColorF(0.0f, 0.8f, 1.0f));
                currentY += 25.0f;
                
                std::string arma = currentPlayer->obterArma() ? currentPlayer->obterArma()->getNameItem() + currentPlayer->obterArma()->obterInfoStatus() : "Punhos";
                std::string escudo = currentPlayer->obterEscudo() ? currentPlayer->obterEscudo()->getNameItem() + currentPlayer->obterEscudo()->obterInfoStatus() : "Nenhum";
                std::string armadura = currentPlayer->obterArmadura() ? currentPlayer->obterArmadura()->getNameItem() + currentPlayer->obterArmadura()->obterInfoStatus() : "Trapos";
                
                box.AddText(L" Weapon    : " + MenuRaycasterUtils::utf8_to_wstring(arma), startX + 20, currentY, 16.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f));
                currentY += 25.0f;
                box.AddText(L" Shield  : " + MenuRaycasterUtils::utf8_to_wstring(escudo), startX + 20, currentY, 16.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f));
                currentY += 25.0f;
                box.AddText(L" Armor: " + MenuRaycasterUtils::utf8_to_wstring(armadura), startX + 20, currentY, 16.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f));
                currentY += 40.0f;
            }
            else if (state == DETALHES) {
                // Arts
                const auto& raceArt = currentPlayer->obterRaca()->getRaceAppearance();
                const auto& classArt = currentPlayer->obterClasse()->obterAparenciaClasseMenu();
                
                float artY = currentY;
                box.AddPixelArt(raceArt, paletaAtributos, startX + 50.0f, artY, 1.5f);
                box.AddPixelArt(classArt, paletaAtributos, startX + 250.0f, artY, 1.5f);
                
                float maxH = std::max(raceArt.size(), classArt.size()) * 1.5f;
                currentY += maxH + 40.0f;
                
                // Not fully implemented text, just simple description
                box.AddText(L"DETALHES DE ATRIBUTOS (Guia):", startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f));
                currentY += 35.0f;
                box.AddText(L"Vida (HP)    : Aumenta sua quantidade maxima de vida.", startX, currentY, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.4f)); currentY += 24.0f;
                box.AddText(L"Forca        : Aumenta o dano base de ataques fisicos.", startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.2f, 0.2f)); currentY += 24.0f;
                box.AddText(L"Destreza     : Aumenta o dano fisico % e define a ordem de turno.", startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.6f, 0.0f)); currentY += 24.0f;
                box.AddText(L"Resistencia  : Reduz o dano recebido de forma fixa.", startX, currentY, 16.0f, D2D1::ColorF(0.2f, 0.4f, 1.0f)); currentY += 24.0f;
                box.AddText(L"Constituicao : Reduz o dano recebido em porcentagem.", startX, currentY, 16.0f, D2D1::ColorF(0.0f, 0.8f, 1.0f)); currentY += 24.0f;
                box.AddText(L"Inteligencia : Aumenta o dano base de ataques magicos.", startX, currentY, 16.0f, D2D1::ColorF(0.8f, 0.2f, 1.0f)); currentY += 24.0f;
                box.AddText(L"Sabedoria    : Aumenta o dano magico % e a potencia de curas.", startX, currentY, 16.0f, D2D1::ColorF(0.6f, 0.4f, 1.0f)); currentY += 35.0f;

            }
            else if (state == SUBIR_NIVEL) {
                box.AddText(L"Escolha o atributo para melhorar:", startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f));
                currentY += 40.0f;
            }
            else if (state == ERRO_NIVEL) {
                box.AddText(L"Voce nao tem XP suficiente para subir de nivel!", startX, currentY, 16.0f, D2D1::ColorF(1.0f, 0.2f, 0.2f));
                currentY += 40.0f;
            }

            // Options rendering
            float opY = optStartY + 50.0f; // Start options at the top on the right side
            for (int i = 0; i < currentSelCount; ++i) {
                std::wstring wOp = MenuRaycasterUtils::utf8_to_wstring(currentOptions[i]);
                D2D1_COLOR_F color = (i == selA) ? D2D1::ColorF(0.2f, 1.0f, 0.2f) : D2D1::ColorF(0.6f, 0.6f, 0.6f);
                MenuRaycasterUtils::adicionarOpcaoMenu(box, wOp, optionsX, opY, (i == selA), color, false);
                opY += 30.0f;
            }
            if (opY > currentY) currentY = opY;
        };

        std::vector<std::string> emptyArte;
        MenuRaycasterUtils::PosicaoArte posArte = MenuRaycasterUtils::PosicaoArte::NENHUMA;
        if (state == PRINCIPAL || state == DETALHES || state == HABILIDADES) {
            posArte = MenuRaycasterUtils::PosicaoArte::ACIMA;
        }

        int res = MenuRaycasterUtils::renderizarMenuPadrao(
            MenuRaycasterUtils::utf8_to_wstring(tituloCaixa),
            D2D1::ColorF(0.6f, 0.2f, 0.8f),
            currentOptions,
            emptyArte,
            {},
            MenuRaycasterUtils::PosicaoArte::NENHUMA,
            1.0f,
            renderCb,
            nullptr,
            ArtesAtributos::logoFicha,
            {}
        );
        
        if (currentSelRef) {
            *currentSelRef = (res != -1) ? res : currentSelCount - 1;
        }

        char c = (res == -1) ? 27 : '\r';
        if (state == PRINCIPAL) {
            if (c == '\r' || c == '\n') {
                if (selecaoAtual == 0) {
                    if (!currentPlayer->podeSubirDeNivel()) state = ERRO_NIVEL;
                    else { state = SUBIR_NIVEL; selecaoSubir = 0; }
                }
                else if (selecaoAtual == 1) state = HABILIDADES;
                else if (selecaoAtual == 2) state = DETALHES;
                else if (selecaoAtual == 3) executando = false;
            } else if (c == 27) {
                executando = false;
            }
        } else if (state == SUBIR_NIVEL) {
            if (c == '\r' || c == '\n') {
                if (selecaoSubir == 7) { 
                    state = PRINCIPAL;
                } else {
                    currentPlayer->subirDeNivel(static_cast<TipoAtributo>(selecaoSubir + 1));
                    state = PRINCIPAL;
                }
            } else if (c == 27) {
                state = PRINCIPAL;
            }
        } else {
            if (c == '\r' || c == '\n' || c == 27) {
                state = PRINCIPAL;
            }
        }
    }
}

void TelaAtributosRaycaster::displayDetalhesAtributos(Character* currentPlayer) {
    if (!currentPlayer) return;
    std::vector<std::string> detalhes;
    detalhes.push_back("--- DETALHES DE ATRIBUTOS E BÔNUS ---");
    detalhes.push_back("Raça: " + currentPlayer->obterRaca()->getRaceName());
    detalhes.push_back("Classe: " + currentPlayer->getNameClasse());
    detalhes.push_back("Força Efetiva: " + std::to_string(currentPlayer->getStrength()));
    detalhes.push_back("Destreza Efetiva: " + std::to_string(currentPlayer->getDexterity()));
    detalhes.push_back("Inteligência Efetiva: " + std::to_string(currentPlayer->getInteligencia()));
    detalhes.push_back("Constituição Efetiva: " + std::to_string(currentPlayer->getConstitution()));
    detalhes.push_back("Resistência Efetiva: " + std::to_string(currentPlayer->getResistance()));
    detalhes.push_back("Sabedoria Efetiva: " + std::to_string(currentPlayer->getWisdom()));
    
    MenuRaycasterUtils::renderizarMenuPadrao(
        L"DETALHES DE ATRIBUTOS",
        D2D1::ColorF(0.6f, 0.2f, 0.8f),
        { "VOLTAR" },
        {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
        nullptr, nullptr, {}, {},
        detalhes
    );
}
