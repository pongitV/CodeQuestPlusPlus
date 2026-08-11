#include "ScreenDiary.h"
#include "ScreenDiaryLayout.h"
#include "ScreenDiaryLogic.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include "../../../core/utils/InputControl.h"
#include "../ScreenBase.h"
#include "../menu/ScreenMenu.h"

#include "../../../systems/inventory/ItemFactory.h"
#include "../../../systems/inventory/Item.h"
#include "../../../systems/progress/Diary.h"
#include "../../../systems/progress/Bestiary.h"

#include "../../../entities/character/Character.h"
#include "../../../entities/races/dwarf/Dwarf.h"
#include "../../../entities/races/elf/Elf.h"
#include "../../../entities/races/human/Human.h"
#include "../../../entities/races/orc/Orc.h"
#include "../../../entities/races/RaceBase.h"
#include "../../../entities/classes/archer/Archer.h"
#include "../../../entities/classes/bard/Bard.h"
#include "../../../entities/classes/warrior/Warrior.h"
#include "../../../entities/classes/mage/Mage.h"
#include "../../../entities/classes/necromancer/Necromancer.h"
#include "../../../entities/classes/ClassBase.h"
#include "../../../entities/npcs/blacksmith/NPCBlacksmithLayout.h"
#include "../../../entities/npcs/merchant/NPCMerchantLayout.h"
#include "../../../entities/npcs/mage-npc/NPCMageNPCLayout.h"
#include "../../../entities/npcs/generic-knight/NPCGenericKnightLayout.h"

#include "../../UIManager.h"
#include "../../../core/utils/Color.h"

#include "../../../core/d2d-context/D2DContext.h"
#include "../../../core/window/GameWindow.h"

namespace {

const int LOGO_HEIGHT = 8;
const int MIN_Y = 11;

enum Secao { PRINCIPAL, BESTIARIO, ITENS, NPCS, RACAS, CLASSES, MISSOES, PROGRESSO };

void adicionarOpcao(std::vector<std::string>& linhas, const std::string& texto, bool selecionado) {
    if (selecionado)
        linhas.push_back(std::string("[OPC] > ") + texto);
    else
        linhas.push_back(std::string("[OPC]   ") + texto);
}

void displayPopupEAguarda(const std::string& titulo, const std::vector<std::string>& linhas) {
    InputControl::limparBuffer();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        GerenciadorPerspectiva::obterDiarioUI().renderizarPopupMensagem(titulo, linhas);
        char c = InputControl::lerTecla();
        if (c != 0 || (GetAsyncKeyState(VK_ESCAPE) & 0x8000) || (GetAsyncKeyState(VK_RETURN) & 0x8000) || (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    InputControl::limparBuffer();
}

void displayPopupComArteEAguarda(const std::string& titulo, const std::vector<std::string>& arte, const std::vector<std::string>& info, const std::string& subtitulo) {
    InputControl::limparBuffer();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        GerenciadorPerspectiva::obterDiarioUI().renderizarPopupInspecaoComArte(titulo, arte, info, subtitulo);
        char c = InputControl::lerTecla();
        if (c != 0 || (GetAsyncKeyState(VK_ESCAPE) & 0x8000) || (GetAsyncKeyState(VK_RETURN) & 0x8000) || (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    InputControl::limparBuffer();
}

void inspecionarItem(Character* currentPlayer, const std::string& nomeItem) {
    auto item = ItemFactory::criarItem(nomeItem);
    if (item) {
        std::vector<std::string> detalhes = item->obterDetalhesInspecao(currentPlayer);
        std::vector<std::string> linhasInsp;
        linhasInsp.push_back(std::string(" >> ") + item->getNameItem() + std::string(" <<"));
        linhasInsp.push_back("");
        linhasInsp.insert(linhasInsp.end(), detalhes.begin(), detalhes.end());
        displayPopupEAguarda("INSPECAO DE ITEM", linhasInsp);
    }
}

void inspecionarNPC(const std::string& nomeNPC) {
    std::vector<std::string> arte;
    std::string lore;
    if (nomeNPC.find("Bjorn") != std::string::npos) {
        arte = NPCBlacksmithLayouts::arteBlacksmith;
        lore = "Bjorn, o Blacksmith da Village.\nUm anao robusto de poucas palavras.\nSempre disposto a melhorar equipamentos.";
    } else if (nomeNPC.find("Franchesco") != std::string::npos) {
        arte = NPCMerchantLayouts::arteMerchant;
        lore = "Franchesco, o Merchant Ambulante.\nSempre com um sorriso no rosto.\nGosta de moedas de ouro mais do que de viver.";
    } else if (nomeNPC.find("Morgana") != std::string::npos) {
        arte = NPCMageNPCLayouts::arteMageNPC;
        lore = "Morgana, a Bruxa da Forest.\nUm misterio, domina alquimia e encantamentos.\nSeu labirinto guarda segredos profundos.";
    } else if (nomeNPC.find("Cavaleiro Real") != std::string::npos) {
        arte = NPCGenericKnightLayouts::arteCavaleiro;
        lore = "Cavaleiro Real.\nProtetores leais do Kingdom.\nFortemente blindados e treinados.";
    } else {
        lore = "Informacoes sobre essa pessoa permanecem um misterio.";
    }
    std::vector<std::string> linhasLore;
    size_t pos = 0;
    std::string temp = lore;
    while ((pos = temp.find('\n')) != std::string::npos) {
        linhasLore.push_back(" > " + temp.substr(0, pos));
        temp.erase(0, pos + 1);
    }
    linhasLore.push_back(" > " + temp);
    displayPopupComArteEAguarda("INSPECAO DE NPC", arte, linhasLore, nomeNPC);
}

void inspecionarRaca(const std::string& nomeRaca) {
    std::unique_ptr<RaceBase> racaObj;
    if (nomeRaca == "Human") racaObj = std::make_unique<Human>();
    else if (nomeRaca == "Dwarf") racaObj = std::make_unique<Dwarf>();
    else if (nomeRaca == "Elf") racaObj = std::make_unique<Elf>();
    else if (nomeRaca == "Ork") racaObj = std::make_unique<Ork>();

    if (racaObj) {
        std::vector<std::string> arte = racaObj->getRaceAppearance();
        std::vector<std::string> attributes = TelaMenu::comporQuadroDeAtributos(
            racaObj->getRaceAttributes(),
            "[ ATRIBUTOS BASE ]",
            "[ HABILIDADE DA RACA ]",
            racaObj->getRaceAbilityName(),
            racaObj->getRaceAbilityDescription()
        );
        displayPopupComArteEAguarda("INSPECAO DE RACA", arte, attributes, nomeRaca);
    }
}

void inspecionarClasse(const std::string& nomeClasse) {
    std::unique_ptr<ClassBase> classeObj;
    if (nomeClasse == "Warrior") classeObj = std::make_unique<Warrior>();
    else if (nomeClasse == "Mage") classeObj = std::make_unique<Mage>();
    else if (nomeClasse == "Archer") classeObj = std::make_unique<Archer>();
    else if (nomeClasse == "Bard") classeObj = std::make_unique<Bard>();
    else if (nomeClasse == "Necromancer") classeObj = std::make_unique<Necromancer>();

    if (classeObj) {
        std::vector<std::string> arte = classeObj->obterAparenciaClasseMenu();
        std::vector<std::string> attributes = TelaMenu::comporQuadroDeAtributos(
            classeObj->obterAtributosClasse(),
            "[ ATRIBUTOS BONUS ]",
            "[ PASSIVA DA CLASSE ]",
            classeObj->getNamePassivaClasse(),
            classeObj->obterDescricaoPassivaClasse(),
            "[ HABILIDADE ATIVA ]",
            classeObj->getNameHabilidadeClasse(),
            classeObj->getClassAbilityDescription()
        );
        displayPopupComArteEAguarda("INSPECAO DE CLASSE", arte, attributes, nomeClasse);
    }
}

void inspecionarBestiario(const std::string& nomeInimigo) {
    const auto* info = Bestiary::instancia().obterInfo(nomeInimigo);
    if (info) {
        std::vector<std::string> detalhes;
        if (Bestiary::instancia().jaDerrotado(nomeInimigo)) {
            detalhes.push_back(std::string("Derrotas: ") + std::to_string(Bestiary::instancia().obterQuantidadeDerrotas(nomeInimigo)) + "");
            detalhes.push_back("");
            auto loreLinhas = TelaDiarioLogic::quebrarTexto(info->lore, 50);
            for (const auto& l : loreLinhas)
                detalhes.push_back("" + l + "");
            auto fatoLinhas = TelaDiarioLogic::quebrarTexto(info->funFact, 50);
            for (const auto& f : fatoLinhas)
                detalhes.push_back("" + f + "");
        } else {
            detalhes.push_back(std::string("Ainda nao derrotado. Pouco se sabe sobre seus costumes."));
        }
        displayPopupComArteEAguarda("BESTIARIO", info->appearance, detalhes, nomeInimigo);
    }
}

void displayRaycaster(Character* currentPlayer) {
    Secao secao = PRINCIPAL;
    int sel = 0;
    int selSub = 0;
    bool executando = true;

    bool emLista = false;
    int idxGrupo = 0;
    std::vector<GrupoCategorizado> grupos;
    std::vector<std::string> listaAtual;

    bool redesenhoCompleto = true;

    while (executando) {
        if (redesenhoCompleto) {
            GerenciadorPerspectiva::obterDiarioUI().renderizarFundo();
        }

        std::vector<std::string> linhas;
        std::string tituloCaixa;
        std::vector<std::string> interativos;

        bool isGroupView = !emLista && (secao == BESTIARIO || secao == ITENS || secao == NPCS || secao == RACAS);

        if (secao == PRINCIPAL) {
            tituloCaixa = " DIARIO DE JORNADA ";
            interativos = {"Bestiario (Inimigos)", "Itens (Descobertos)", "NPCs Conhecidos",
                           "Racas do Mundo", "Classes Jogaveis", "Missoes (Diario)",
                           "Progress e Feitos", "Voltar"};
            for (size_t i = 0; i < interativos.size(); ++i)
                adicionarOpcao(linhas, interativos[i], (int)i == sel);
        } else if (isGroupView) {
            if (secao == BESTIARIO) {
                tituloCaixa = " BESTIARIO ";
                grupos = TelaDiarioLogic::categorizarBestiario();
            } else if (secao == ITENS) {
                tituloCaixa = " ITENS DESCOBERTOS ";
            } else if (secao == NPCS) {
                tituloCaixa = " NPCS CONHECIDOS ";
                grupos = TelaDiarioLogic::categorizarNPCs();
            } else if (secao == RACAS) {
                tituloCaixa = " RACAS DESCOBERTAS ";
                auto todasRacas = Diary::instancia().obterRacasDescobertas();
                grupos = TelaDiarioLogic::categorizarRacas(todasRacas);
            }

            if (secao == ITENS) {
                auto cats = TelaDiarioLogic::categorizarItens(currentPlayer);
                struct CatInfo { const char* nome; std::vector<std::string>* lista; };
                CatInfo todasCats[] = {
                    {"Armas", &cats.armas}, {"Escudos", &cats.escudos},
                    {"Armaduras", &cats.armaduras}, {"Consumiveis", &cats.consumiveis},
                    {"Materiais", &cats.materiais}, {"Missoes", &cats.missoes},
                    {"Outros", &cats.outros}
                };
                grupos.clear();
                for (auto& ci : todasCats) {
                    if (!ci.lista->empty())
                        grupos.push_back({ci.nome, *ci.lista});
                }
            }

            if (grupos.empty()) {
                linhas.push_back("   " + std::string("Nenhum registro encontrado ainda."));
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == 0);
            } else {
                for (size_t i = 0; i < grupos.size(); ++i) {
                    interativos.push_back(grupos[i].nome);
                    std::string txt = grupos[i].nome + " (" + std::to_string(grupos[i].itens.size()) + ")";
                    adicionarOpcao(linhas, txt, (int)i == selSub);
                }
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == (int)grupos.size());
            }
        } else {
            if (secao == MISSOES) tituloCaixa = " DIARIO DE MISSOES ";
            else if (secao == PROGRESSO) tituloCaixa = " PROGRESSO E FEITOS ";
            else if (secao == CLASSES) tituloCaixa = " CLASSES JOGAVEIS ";
            else if (secao == BESTIARIO) tituloCaixa = grupos.empty() ? " BESTIARIO " : (" " + grupos[idxGrupo].nome + " ");
            else if (secao == ITENS) tituloCaixa = grupos.empty() ? " ITENS " : (" " + grupos[idxGrupo].nome + " ");
            else if (secao == NPCS) tituloCaixa = grupos.empty() ? " NPCS " : (" " + grupos[idxGrupo].nome + " ");
            else if (secao == RACAS) tituloCaixa = grupos.empty() ? " RACAS " : (" " + grupos[idxGrupo].nome + " ");

            if (secao == PROGRESSO) {
                auto dados = TelaDiarioLogic::obterProgresso();
                if (dados.totalConquistas == 0) {
                    linhas.push_back("   " + std::string("Nenhum grande feito para registrar ainda..."));
                } else {
                    for (const auto& l : dados.linhasNPC) linhas.push_back(l);
                    for (const auto& l : dados.linhasMonstro) linhas.push_back(l);
                    for (const auto& l : dados.linhasItem) linhas.push_back(l);
                    if (!linhas.empty()) linhas.pop_back();
                }
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == 0);
            } else if (secao == MISSOES) {
                auto cats = TelaDiarioLogic::categorizarMissoes(currentPlayer);
                linhas.push_back(std::string("Em andamento"));
                if (cats.emAndamento.empty()) linhas.push_back("  (Nenhuma)");
                for (const auto& m : cats.emAndamento) linhas.push_back(std::string("  ") + m + "");
                linhas.push_back("");
                linhas.push_back(std::string("Prontas"));
                if (cats.prontas.empty()) linhas.push_back("  (Nenhuma)");
                for (const auto& m : cats.prontas) linhas.push_back(std::string("  ") + m + "");
                linhas.push_back("");
                linhas.push_back(std::string("Completas"));
                if (cats.completas.empty()) linhas.push_back("  (Nenhuma)");
                for (const auto& m : cats.completas) linhas.push_back(std::string("  ") + m + "");
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == 0);
            } else {
                if (secao == CLASSES) {
                    listaAtual = TelaDiarioLogic::obterTodasClasses();
                } else if (!grupos.empty() && idxGrupo < (int)grupos.size()) {
                    listaAtual = grupos[idxGrupo].itens;
                }

                if (listaAtual.empty()) {
                    linhas.push_back("   " + std::string("Nenhum registro encontrado ainda."));
                } else {
                    for (size_t i = 0; i < listaAtual.size(); ++i) {
                        interativos.push_back(listaAtual[i]);
                        adicionarOpcao(linhas, listaAtual[i], (int)i == selSub);
                    }
                }
                linhas.push_back("");
                interativos.push_back("Voltar");
                adicionarOpcao(linhas, "Voltar", selSub == (int)listaAtual.size());
            }
        }

        int totalOpcoes = interativos.size();

        if (secao == PRINCIPAL) {
            if (sel >= totalOpcoes && totalOpcoes > 0) sel = totalOpcoes - 1;
        } else {
            if (selSub >= totalOpcoes && totalOpcoes > 0) selSub = totalOpcoes - 1;
        }

        std::vector<std::string> caixaPrevia = TelaBase::criarCaixa(linhas, tituloCaixa, 0, Color::YELLOW, "");
        int startYCaixa = (40 - (int)caixaPrevia.size()) / 2;
        if (startYCaixa < MIN_Y) startYCaixa = MIN_Y;
        if (startYCaixa < 8) startYCaixa = 8;

        GerenciadorPerspectiva::obterDiarioUI().displayCabecalho(startYCaixa);
        GerenciadorPerspectiva::obterDiarioUI().renderizarCaixa(linhas, tituloCaixa, Color::YELLOW, MIN_Y, startYCaixa);

        redesenhoCompleto = false;
        char tecla = InputControl::lerTecla();

        auto moverCursor = [&](int dir) {
            int& s = (secao == PRINCIPAL) ? sel : selSub;
            s += dir;
            if (s < 0) s = totalOpcoes - 1;
            if (s >= totalOpcoes) s = 0;
        };

        if (tecla == 'w' || tecla == 'W') moverCursor(-1);
        else if (tecla == 's' || tecla == 'S') moverCursor(1);
        else if (tecla == 27 || tecla == 'c' || tecla == 'C' || tecla == 'i' || tecla == 'I' || tecla == 'j' || tecla == 'J') {
            executando = false;
            break;
        }
        else if (tecla == '\n' || tecla == '\r') {
            redesenhoCompleto = true;
            if (secao == PRINCIPAL) {
                switch (sel) {
                    case 0: secao = BESTIARIO; break;
                    case 1: secao = ITENS; break;
                    case 2: secao = NPCS; break;
                    case 3: secao = RACAS; break;
                    case 4: secao = CLASSES; break;
                    case 5: secao = MISSOES; break;
                    case 6: secao = PROGRESSO; break;
                    case 7: executando = false; break;
                }
                emLista = false;
                selSub = 0;
                idxGrupo = 0;
                grupos.clear();
                listaAtual.clear();
            } else if (isGroupView) {
                if (selSub == (int)grupos.size()) {
                    secao = PRINCIPAL;
                    emLista = false;
                } else if (selSub >= 0 && selSub < (int)grupos.size()) {
                    idxGrupo = selSub;
                    emLista = true;
                    selSub = 0;
                }
    } else {
            int ultimoIdx = (int)listaAtual.size();
            if (selSub == ultimoIdx || listaAtual.empty()) {
                if (secao == MISSOES || secao == PROGRESSO || secao == CLASSES) {
                    secao = PRINCIPAL;
                } else {
                    emLista = false;
                    selSub = 0;
                }
                } else if (selSub >= 0 && selSub < ultimoIdx) {
                    std::string itemSel = listaAtual[selSub];
                    if (secao == ITENS) {
                        std::string nomeItem = (itemSel.size() > 3) ? itemSel.substr(3) : itemSel;
                        inspecionarItem(currentPlayer, nomeItem);
                    } else if (secao == NPCS) {
                        inspecionarNPC(itemSel);
                    } else if (secao == RACAS) {
                        inspecionarRaca(itemSel);
                    } else if (secao == CLASSES) {
                        inspecionarClasse(itemSel);
                    } else if (secao == BESTIARIO) {
                        inspecionarBestiario(itemSel);
                    }
                }
            }
        }
    }
}

} // anonymous namespace

void TelaDiario::display(Character* currentPlayer) {
    if (!currentPlayer) return;

    displayRaycaster(currentPlayer);
}
