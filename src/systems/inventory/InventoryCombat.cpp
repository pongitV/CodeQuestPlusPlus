#include "InventoryCombat.h"
#include "../../core/d2d-context/D2DContext.h"
#include "../../core/window/GameWindow.h"
#include "../../ui/UIManager.h"
#include "../../entities/character/Character.h"
#include "Item.h"
#include "./equipment/WeaponEquipment.h"
#include "./equipment/ShieldEquipment.h"
#include "./equipment/ArmorEquipment.h"
#include "../../core/utils/InputControl.h"
#include "../../ui/screens/ScreenBase.h"
#include "../../ui/screens/inventory/ScreenInventory.h"
#include "../../ui/screens/inventory/ScreenInventoryLayout.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "../../core/utils/DialogFunctions.h"
#include "InventoryController.h"
#include "../../rendering/raycaster/engine-raycaster/RaycasterFrame.h"
#include "../../rendering/raycaster/screens/utils/MenuD2DUtils.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include "../../core/utils/Color.h"

enum EstadoInventario { PRINCIPAL, ARSENAL, CONSUMIVEIS, ESTOQUE, MISSAO };

static int lerSelecaoPopupInventario(const std::string& titulo, const std::string& mensagem, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes) {
    // Renderização Direct2D
        auto renderCb = [&](UIDynamicBox& box, int selA, float optStartX, float optStartY, float pixelScale) {
            float lw = UIRenderer2D::LOGICAL_WIDTH;
            
            float currentY = optStartY;
            if (!mensagem.empty()) {
                std::wstring wMsg = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(mensagem));
                box.AddText(wMsg, lw/2.0f, currentY, 18.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
                currentY += 40.0f;
            }
            
            for (const auto& t : texto) {
                std::wstring wT = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(t));
                box.AddText(wT, lw/2.0f, currentY, 16.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), true);
                currentY += 25.0f;
            }
            
            currentY += 30.0f;
            for (size_t i = 0; i < opcoes.size(); ++i) {
                std::wstring wOp = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(opcoes[i]));
                MenuRaycasterUtils::adicionarOpcaoMenu(box, wOp, lw/2.0f, currentY, ((int)i == selA), D2D1::ColorF(0.7f, 0.7f, 0.7f), true);
                currentY += 30.0f;
            }
        };

    int res = MenuRaycasterUtils::renderizarMenuPadrao(
        MenuRaycasterUtils::utf8_to_wstring(titulo),
        D2D1::ColorF(1.0f, 1.0f, 0.0f),
        opcoes,
        {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
        renderCb,
        nullptr,
        ArtesInventario::logoInventario,
        {}
    );
    return (res == -1) ? opcoes.size() - 1 : res;
}


static void displayMensagemPopupInventario(const std::string& titulo, const std::vector<std::string>& texto) {
    lerSelecaoPopupInventario(titulo, "", texto, {"[ VOLTAR ]"});
}

static void displayResultadoItem(const UsoItemInfo& info, Item* item, bool* turnoFoiConsumido) {
    switch (info.resultado) {
        case ResultadoItem::Erro_TurnoJaUsado:
            displayMensagemPopupInventario("SISTEMA", {"Voce ja usou um item neste turno!"});
            break;
        case ResultadoItem::Erro_EscudoQuebrado: {
            std::string msg = DialogFunctions::formatarMsgSistema("O escudo [" + info.nomeItem + "] esta quebrado e nao pode ser equipado!", Color::RED);
            displayMensagemPopupInventario("SISTEMA", {msg});
            break;
        }
        case ResultadoItem::Erro_Requisitos:
            displayMensagemPopupInventario("SISTEMA", {info.mensagemExtra});
            break;
        case ResultadoItem::Desequipou:
            displayMensagemPopupInventario("SISTEMA", {info.nomeItem + " desequipado(a)!"});
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            displayMensagemPopupInventario("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ResultadoItem::Equipou:
            displayMensagemPopupInventario("SISTEMA", {info.nomeItem + " equipado(a)!"});
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            displayMensagemPopupInventario("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ResultadoItem::Usou_Turno:
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            break;
        case ResultadoItem::Usou_SemTurno:
            break;
        case ResultadoItem::Erro_NaoPodeUsar:
            displayMensagemPopupInventario("SISTEMA", {ControleInventario::obterMensagemErro(item, turnoFoiConsumido != nullptr)});
            break;
        default: break;
    }
}

static int lerInteiroPopupInventario(const std::string& titulo, const std::string& mensagem, int min, int max) {
    std::string currentInput = "";
    // D2D Rendering
    while (true) {
            auto renderCb = [&](UIDynamicBox& box, int selA, float lw, float lh) {
                std::wstring wTit = MenuRaycasterUtils::utf8_to_wstring(titulo);
                box.SetTitle(wTit, D2D1::ColorF(1.0f, 1.0f, 0.0f));
                
                std::wstring wMsg = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(mensagem));
                box.AddText(wMsg, lw/2.0f, 90.0f, 18.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
                
                std::wstring wInput = MenuRaycasterUtils::utf8_to_wstring(" >> " + currentInput + "_");
                box.AddText(wInput, lw/2.0f, 130.0f, 20.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f), true);
                
                box.AddText(L"[ENTER para confirmar]", lw/2.0f, 170.0f, 14.0f, D2D1::ColorF(0.6f, 0.6f, 0.6f), true);
            };
            
            auto* d2d = D2DContext::renderer;
            if (d2d) {
                UIDynamicBox box;
                float lw = UIRenderer2D::LOGICAL_WIDTH;
                float lh = UIRenderer2D::LOGICAL_HEIGHT;
                renderCb(box, 0, lw, lh);
                
                if (D2DContext::window) D2DContext::window->processarMensagens();
                InputControl::atualizarTeclas();
                
                d2d->obterRenderTarget()->BeginDraw();
                RaycasterFrame::restaurarUltimoQuadro();
                box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.95f, 2.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f), 20.0f, lw/2.0f, lh/2.0f);
                d2d->obterRenderTarget()->EndDraw();
                d2d->apresentarBackbuffer();
            }
            
            char tecla = InputControl::lerTecla();
            if (tecla >= '0' && tecla <= '9') {
                currentInput += tecla;
            } else if (tecla == 8 && !currentInput.empty()) { // backspace
                currentInput.pop_back();
            } else if (tecla == '\r' || tecla == '\n') {
                if (currentInput.empty()) return min;
                int val = std::stoi(currentInput);
                if (val < min) return min;
                if (val > max) return max;
                return val;
            }
        }
    }


void InventarioCombate::gerenciarInventario(Character* currentPlayer, bool* turnoFoiConsumido)
{
    if (currentPlayer == nullptr) return;
    
    EstadoInventario state = PRINCIPAL;
    int selecaoAtual = 0;
    int selecaoSub = 0;
    bool executando = true;
    
    std::vector<Item*> mapIndexParaItem;

    bool redesenhoCompletoInv = true;

    while (executando) {
        bool is3D = GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
        
        std::vector<std::string> linhas;
        std::string tituloCaixa = "";
        std::vector<std::string> interativos;
        std::vector<int> indicesReais;
        
        if (state == PRINCIPAL) {
            tituloCaixa = " MENU DE BOLSOS ";
            std::string strBolso = "BOLSO: " + std::to_string(currentPlayer->obterInventario()->obterOuro()) + " Moedas de Ouro [$$]";
            
            std::vector<std::string> opcoesBase;
            if (currentPlayer->obterConsumivelRapido()) {
                int qtd = currentPlayer->obterInventario()->contarItem(currentPlayer->obterConsumivelRapido()->getNameItem());
                opcoesBase.push_back(std::string("[+] ") + "Acesso Rapido: " + currentPlayer->obterConsumivelRapido()->getNameItem() + " (" + std::to_string(qtd) + "x)");
            }
            opcoesBase.push_back("Arsenal de Equipamentos");
            opcoesBase.push_back("Itens Consumiveis");
            opcoesBase.push_back("Estoque e Materiais");
            opcoesBase.push_back("Itens de Missao");
            opcoesBase.push_back("");
            opcoesBase.push_back(strBolso);
            opcoesBase.push_back("");
            opcoesBase.push_back("VOLTAR");
            
            for (size_t i = 0; i < opcoesBase.size(); ++i) {
                if (opcoesBase[i].empty() || opcoesBase[i].find("BOLSO:") != std::string::npos || opcoesBase[i].substr(0, 3) == "   ") {
                    linhas.push_back("   " + opcoesBase[i]);
                } else {
                    interativos.push_back(opcoesBase[i]);
                    indicesReais.push_back(i);
                    if (interativos.size() - 1 == selecaoAtual) {
                        linhas.push_back(std::string(" > ") + opcoesBase[i]);
                    } else {
                        linhas.push_back("   " + opcoesBase[i]);
                    }
                }
            }
        } else if (state == ARSENAL) {
            tituloCaixa = " ARSENAL DE EQUIPAMENTOS ";

            mapIndexParaItem.clear();

            Item* armaEq = currentPlayer->obterArma();
            Item* armaduraEq = currentPlayer->obterArmadura();
            Item* escudoEq = currentPlayer->obterEscudo();

            auto todosItens = currentPlayer->obterInventario()->obterTodosOsItens();
            std::vector<Item*> armas, armaduras, escudos;
            for (auto* item : todosItens) {
                if (item == armaEq || item == armaduraEq || item == escudoEq) continue;
                TipoEquipamento tipo = item->obterTipo();
                if (tipo == TipoEquipamento::ARMA) armas.push_back(item);
                else if (tipo == TipoEquipamento::ARMADURA) armaduras.push_back(item);
                else if (tipo == TipoEquipamento::ESCUDO) escudos.push_back(item);
            }


            std::string corDiv = "";
            std::string corReset = "";

            auto adicionarItem = [&](const std::string& nome, Item* item) {
                int idx = (int)interativos.size();
                interativos.push_back(nome);
                indicesReais.push_back((int)mapIndexParaItem.size());
                mapIndexParaItem.push_back(item);
                if (idx == selecaoSub)
                    linhas.push_back(std::string(" > ") + nome);
                else
                    linhas.push_back("   " + nome);
            };

            auto adicionarGrupo = [&](const std::string& label, std::vector<Item*>& grupo) {
                if (grupo.empty()) return;
                linhas.push_back(" " + corDiv + "--- " + label + " ---" + corReset);
                for (auto* item : grupo) {
                    auto itensAgrupados = currentPlayer->obterInventario()->contarItem(item->getNameItem());
                    std::string prefixo = (itensAgrupados > 1) ? std::to_string(itensAgrupados) + "x " : "";
                    adicionarItem(prefixo + item->getNameItem(), item);
                }
                linhas.push_back("");
            };

            // Equipados (não interativo)
            linhas.push_back(" " + corDiv + "--- Equipados ---" + corReset);
            bool temEq = false;
            auto addEq = [&](const std::string& label, Item* item) {
                if (!item) return;
                temEq = true;
                std::string nome = item->getNameItem();
                // também adiciona aos interativos para permitir seleção
                int idx = (int)interativos.size();
                interativos.push_back("(E) " + nome);
                indicesReais.push_back((int)mapIndexParaItem.size());
                mapIndexParaItem.push_back(item);
                if (idx == selecaoSub)
                    linhas.push_back(std::string(" > ") + std::string("[E] ") + label + ": " + nome);
                else
                    linhas.push_back("   " + std::string("[E] ") + label + ": " + nome);
            };
            addEq("Weapon", armaEq);
            addEq("Armor", armaduraEq);
            addEq("Shield", escudoEq);
            if (!temEq) linhas.push_back("   " + std::string("(Nada equipado)") + corReset);
            linhas.push_back("");

            adicionarGrupo("Armas", armas);
            adicionarGrupo("Armaduras", armaduras);
            adicionarGrupo("Escudos", escudos);

            interativos.push_back("VOLTAR");
            indicesReais.push_back(-1);
            if ((int)interativos.size() - 1 == selecaoSub)
                linhas.push_back(" > VOLTAR");
            else
                linhas.push_back("   VOLTAR");

        } else {
            int categoria = 0;
            if (state == CONSUMIVEIS) { tituloCaixa = " ITENS CONSUMIVEIS "; categoria = 1; }
            else if (state == ESTOQUE) { tituloCaixa = " ESTOQUE E MATERIAIS "; categoria = 2; }
            else if (state == MISSAO) { tituloCaixa = " ITENS DE MISSAO "; categoria = 3; }

            auto itens = TelaInventario::obterListaCategoria(currentPlayer, categoria, false);
            mapIndexParaItem.clear();

            if (!itens.empty()) {
                for (const auto& p : itens) {
                    interativos.push_back(p.first);
                    indicesReais.push_back((int)mapIndexParaItem.size());
                    mapIndexParaItem.push_back(p.second);
                }
            }
            interativos.push_back("VOLTAR");
            indicesReais.push_back(-1);
        }
        
        int totalOpcoes = interativos.size();
        int* selRef = (state == PRINCIPAL) ? &selecaoAtual : &selecaoSub;
        if (*selRef >= totalOpcoes && totalOpcoes > 0) *selRef = totalOpcoes - 1;

        std::vector<GrupoCorUI> paletaTitulo = {
            {"█", 100, 200, 100},
            {"░", 50, 100, 50},
            {"_", 100, 200, 100},
            {"|", 100, 200, 100},
            {"-", 100, 200, 100}
        };

        auto construtor = [&](UIDynamicBox& box, int selA, float optStartX, float optStartY, float pixelScale) {
            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            float currentY = 80.0f;
            int interactiveCounter = 0;

            for (const auto& l : linhas) {
                std::string plain = l;

                std::string renderText = plain;
                D2D1_COLOR_F color = D2D1::ColorF(0.8f, 0.8f, 0.8f);

                if (plain.length() >= 3 && (plain.substr(0, 3) == " > " || plain.substr(0, 3) == "   ")) {
                    std::string content = plain.substr(3);
                    if (!content.empty() && content.find("BOLSO:") == std::string::npos && content.find("(Nada equipado)") == std::string::npos && content.find("Nenhum item") == std::string::npos) {
                        std::wstring wContent = MenuRaycasterUtils::utf8_to_wstring(content);
                        MenuRaycasterUtils::adicionarOpcaoMenu(box, wContent, 50.0f, currentY, (interactiveCounter == selA), D2D1::ColorF(0.8f, 0.8f, 0.8f), false);
                        interactiveCounter++;
                        currentY += 25.0f;
                        continue;
                    } else {
                        renderText = content;
                        if (content.find("---") != std::string::npos) color = D2D1::ColorF(0.5f, 0.5f, 0.5f);
                    }
                } else {
                    if (plain.find("---") != std::string::npos) color = D2D1::ColorF(0.5f, 0.5f, 0.5f);
                }

                std::wstring wLine = MenuRaycasterUtils::utf8_to_wstring(renderText);
                box.AddText(wLine, 50.0f, currentY, 16.0f, color, false);
                currentY += 25.0f;
            }
            
            currentY += 20.0f;
            box.AddText(L"[ENTER] Selecionar  [ESC] Voltar  [I] Inspecionar  [Q] Equipar  [F] Largar", logicalW / 2.0f, currentY, 14.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f), true);
        };

        auto extraHandler = [&](char tecla, int& selA) -> bool {
            if (tecla == 'q' || tecla == 'Q') {
                if (state != PRINCIPAL && selA < (int)indicesReais.size() && indicesReais[selA] != -1) {
                    Item* itemParaEquipar = mapIndexParaItem[indicesReais[selA]];
                    ControleInventario::usarOuEquipar(currentPlayer, itemParaEquipar, false);
                }
                return true;
            }
            if (tecla == 'i' || tecla == 'I') {
                if (state != PRINCIPAL && selA < (int)indicesReais.size() && indicesReais[selA] != -1) {
                    Item* itemParaInspecionar = mapIndexParaItem[indicesReais[selA]];
                    MenuRaycasterUtils::renderizarPopupCaixa({}, paletaTitulo, 1, [&](UIDynamicBox& ibox, int, float lw, float lh){
                        std::wstring wName = MenuRaycasterUtils::utf8_to_wstring(itemParaInspecionar->getNameItem());
                        ibox.SetTitle(L"INSPECAO: " + wName, D2D1::ColorF(1.0f, 1.0f, 0.0f));
                        
                        std::vector<std::string> detalhes = itemParaInspecionar->obterDetalhesInspecao(currentPlayer);
                        float textY = 120.0f;
                        for (const auto& det : detalhes) {
                            std::wstring wDet = MenuRaycasterUtils::utf8_to_wstring(det);
                            ibox.AddText(wDet, lw/2.0f, textY, 18.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f), true);
                            textY += 30.0f;
                        }
                        
                        MenuRaycasterUtils::adicionarOpcaoMenu(ibox, L"VOLTAR", lw/2.0f, textY + 40.0f, true, D2D1::ColorF(0.7f, 0.7f, 0.7f), true);
                    }, nullptr);
                }
                return true;
            }
            if (tecla == 'f' || tecla == 'F') {
                if (state != PRINCIPAL && selA < (int)indicesReais.size() && indicesReais[selA] != -1) {
                    Item* itemParaLargar = mapIndexParaItem[indicesReais[selA]];
                    currentPlayer->obterInventario()->removerItem(itemParaLargar);
                }
                return true;
            }
            return false;
        };

        std::vector<std::string> emptyArte;
        std::vector<std::string> emptyOpcoes(totalOpcoes);
        int res = MenuRaycasterUtils::renderizarMenuPadrao(
            MenuRaycasterUtils::utf8_to_wstring(tituloCaixa),
            D2D1::ColorF(1.0f, 1.0f, 0.0f),
            emptyOpcoes,
            emptyArte,
            paletaTitulo,
            MenuRaycasterUtils::PosicaoArte::NENHUMA,
            1.0f,
            construtor,
            extraHandler,
            ArtesInventario::logoInventario,
            {}
        );
        
        *selRef = (res != -1) ? res : totalOpcoes - 1;
        char tecla = (res == -1) ? 27 : '\r';

        if (tecla == 's' || tecla == 'S') { // Keep for compatibility with remaining block if any
            (*selRef)++;
        } else if (tecla == '\n' || tecla == '\r') {
            redesenhoCompletoInv = true;
            if (totalOpcoes > 0) {
                if (state == PRINCIPAL) {
                    int offset = currentPlayer->obterConsumivelRapido() ? 1 : 0;
                    int escLogica = indicesReais[*selRef];
                    
                    if (escLogica == 7 + offset) {
                        executando = false;
                    } else if (offset == 1 && escLogica == 0) {
                        // Consumable rapido
                        Item* rapido = currentPlayer->obterConsumivelRapido();
                        std::string nomeRapido = rapido->getNameItem();
                        int countAntes = currentPlayer->obterInventario()->contarItem(nomeRapido);
                        if (countAntes > 0) {
                            bool turnoJaUsado = turnoFoiConsumido && *turnoFoiConsumido;
                            UsoItemInfo info = ControleInventario::usarOuEquipar(currentPlayer, rapido, turnoJaUsado);
                            if (turnoFoiConsumido && info.consumiuTurno) *turnoFoiConsumido = true;
                            if (currentPlayer->obterItemSelecionadoParaUso() != nullptr) {
                                executando = false;
                            }
                            if (currentPlayer->obterInventario()->contarItem(nomeRapido) == 0) {
                                currentPlayer->desequiparConsumivel();
                            }
                        } else {
                            currentPlayer->desequiparConsumivel();
                        }
                        if (turnoFoiConsumido && *turnoFoiConsumido) executando = false;
                        
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    } else {
                        int cat = escLogica - offset;
                        if (cat == 0) state = ARSENAL;
                        else if (cat == 1) state = CONSUMIVEIS;
                        else if (cat == 2) state = ESTOQUE;
                        else if (cat == 3) state = MISSAO;
                        selecaoSub = 0;
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    }
                } else {
                    int idx = indicesReais[*selRef];
                    if (idx == -1) {
                        state = PRINCIPAL;
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    } else {
                        Item* itemEncontrado = mapIndexParaItem[idx];
                        bool ehEquipavel = itemEncontrado->isEquipavel();
                        
                        bool submenuAberto = true;
                        while(submenuAberto) {
                            int subOpcao = lerSelecaoPopupInventario(
                                "OPCOES DE ITEM", 
                                "",
                                {"O que deseja fazer com:", std::string(">> ") + itemEncontrado->getNameItem() + std::string(" <<")}, 
                                {"Usar / Equipar", "Inspecionar", "[ VOLTAR ]"}
                            );
                            
                            if (subOpcao == 2) { // VOLTAR
                                submenuAberto = false;
                                if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                                break;
                            } else if (subOpcao == 0) { // Usar / Equipar
                                bool turnoJaUsado = turnoFoiConsumido && *turnoFoiConsumido;
                                if (ehEquipavel) {
                                    UsoItemInfo info = ControleInventario::usarOuEquipar(currentPlayer, itemEncontrado, turnoJaUsado);
                                    displayResultadoItem(info, itemEncontrado, turnoFoiConsumido);
                                } else {
                                    int qtdDisponivel = currentPlayer->obterInventario()->contarItem(itemEncontrado->getNameItem());
                                    int amountParaUsar = 1;
                                    
                                    if (qtdDisponivel > 1) {
                                        int escolhaQtd = lerSelecaoPopupInventario(
                                            "QUANTIDADE: " + itemEncontrado->getNameItem(),
                                            "",
                                            {"Voce possui " + std::to_string(qtdDisponivel) + " unidades deste item."},
                                            {"Usar UMA unidade", "Usar TODAS as unidades", "Usar amount ESPECIFICA", "[ CANCELAR ]"}
                                        );
                                        
                                        if (escolhaQtd == 0) {
                                            amountParaUsar = 1;
                                        } else if (escolhaQtd == 1) {
                                            amountParaUsar = qtdDisponivel;
                                        } else if (escolhaQtd == 2) {
                                            std::string msgQtd = "Quantidade (1 a " + std::to_string(qtdDisponivel) + ", 0 cancelar): ";
                                            amountParaUsar = lerInteiroPopupInventario("QUANTIDADE", msgQtd, 0, qtdDisponivel);
                                        } else {
                                            if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                                            continue; 
                                        }
                                    }
                                    
                                    if (amountParaUsar <= 0) {
                                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                                        continue;
                                    }
                                    
                                    std::string nomeItem = itemEncontrado->getNameItem();
                                    int countAntes = currentPlayer->obterInventario()->contarItem(nomeItem);
                                    bool consumiuAlgumTurno = false;
                                    for (int i = 0; i < amountParaUsar; ++i) {
                                        Item* itemAtual = nullptr;
                                        for (auto* it : currentPlayer->obterInventario()->obterTodosOsItens()) {
                                            if (it && it->getNameItem() == nomeItem) {
                                                itemAtual = it;
                                                break;
                                            }
                                        }
                                        if (!itemAtual) break;



                                        turnoJaUsado = turnoFoiConsumido && *turnoFoiConsumido;
                                        UsoItemInfo info = ControleInventario::usarOuEquipar(currentPlayer, itemAtual, turnoJaUsado);
                                        if (info.consumiuTurno) consumiuAlgumTurno = true;
                                        
                                        if (currentPlayer->obterItemSelecionadoParaUso() != nullptr) {
                                            if (amountParaUsar > 1) {
                                                displayMensagemPopupInventario("SISTEMA", {"Este item requer selecao de alvo e", "sera usado apenas uma vez."});
                                            }
                                            break;
                                        }
                                        
                                        int countDepois = currentPlayer->obterInventario()->contarItem(nomeItem);
                                        if (countDepois == countAntes && !ehEquipavel) {
                                            break;
                                        }
                                    }

                                    
                                    if (currentPlayer->obterConsumivelRapido() && currentPlayer->obterInventario()->contarItem(currentPlayer->obterConsumivelRapido()->getNameItem()) == 0) {
                                        currentPlayer->desequiparConsumivel();
                                    }

                                    if (turnoFoiConsumido && consumiuAlgumTurno) {
                                        *turnoFoiConsumido = true;
                                    }
                                }
                                submenuAberto = false; 
                                if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                            } else if (subOpcao == 1) { // Inspecionar
                                std::vector<std::string> detalhes = itemEncontrado->obterDetalhesInspecao(currentPlayer);
                                std::vector<std::string> linhasInsp;
                                linhasInsp.push_back(std::string(" >> ") + itemEncontrado->getNameItem() + std::string(" <<"));
                                linhasInsp.push_back("");
                                linhasInsp.insert(linhasInsp.end(), detalhes.begin(), detalhes.end());
                                
                                displayMensagemPopupInventario("INSPECAO DE ITEM", linhasInsp);
                                if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                            }
                        }
                        
                        if (turnoFoiConsumido && *turnoFoiConsumido) executando = false;
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    }
                }
            }
        }
    }
}


