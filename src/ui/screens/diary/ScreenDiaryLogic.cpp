#include "ScreenDiaryLogic.h"
#include "../../../systems/progress/Progression.h"
#include "../../../systems/progress/ProgressionFlags.h"
#include "../../../systems/progress/Diary.h"
#include "../../../systems/progress/Bestiary.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../systems/inventory/Item.h"
#include "../../../entities/character/Character.h"
#include <algorithm>
#include <map>
#include <functional>
#include <sstream>

namespace {
    enum class CategoriaProgresso { NPC, MONSTRO, ITEM };

    struct ItemProgresso {
        const char* flag;
        const char* nome;
        const char* descricao;
        CategoriaProgresso categoria;
    };

    const std::vector<ItemProgresso> itensDeProgresso = {
        {Flags::Vila_BjornResgatado, "O Salvador da Forja", "Resgatou o ferreiro Bjorn encurralado por um Orc.", CategoriaProgresso::NPC},
        {Flags::Vila_ConviteReal, "Passe Real", "Ajudou os cavaleiros a se livrarem dos Trolls e recebeu um convite para o Kingdom.", CategoriaProgresso::NPC},
        {Flags::Floresta_MissaoMorgana, "Pacto com a Bruxa", "Entregou os Coracoes da Forest para Morgana e recebeu a chave para o Labirinto.", CategoriaProgresso::NPC},
        {Flags::Floresta_MahoragaDerrotado, "Ritual concluido", "Derrotou Mahoraga pela primeira vez.", CategoriaProgresso::MONSTRO},
        {Flags::PonteReino_TrollDerrotado, "Pacificador do Kingdom", "Derrotou todos os Trolls que invadiram a entrada do Kingdom.", CategoriaProgresso::MONSTRO}
    };

    struct MissaoRegistro {
        std::string id;
        std::string nome;
        std::function<bool(Character*)> checarRequisitos;
    };

    const std::vector<MissaoRegistro> registroDeMissoes = {
        {
            "morgana_coracoes",
            "Consiga 3x Coracoes da floresta (Morgana)",
            [](Character* p) { return p->obterInventario()->contarItem("Coracao da floresta") >= 3; }
        },
        {
            "cavaleiro_trolls",
            "Reportar Trolls derrotados (Cavaleiro Real)",
            [](Character*) { return Progression::instancia().obterFlag(Flags::PonteReino_TrollDerrotado); }
        }
    };
}

DadosProgresso TelaDiarioLogic::obterProgresso() {
    DadosProgresso dados;
    int conquistasExibidas = 0;

    auto processarCategoria = [&](const std::string& titulo, CategoriaProgresso categoria) -> int {
        std::vector<std::string>* alvo = nullptr;
        if (categoria == CategoriaProgresso::NPC) alvo = &dados.linhasNPC;
        else if (categoria == CategoriaProgresso::MONSTRO) alvo = &dados.linhasMonstro;
        else if (categoria == CategoriaProgresso::ITEM) alvo = &dados.linhasItem;

        int count = 0;
        for (const auto& item : itensDeProgresso) {
            if (item.categoria == categoria && Progression::instancia().obterFlag(item.flag)) {
                if (alvo->empty()) {
                    alvo->push_back("  " + std::string("\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 ") + titulo + std::string(" \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90"));
                    alvo->push_back("");
                }
                std::string corNome = "";
                std::string corDesc = "";
                std::string status = std::string("[CONCLUIDO]");

                alvo->push_back("    " + status + " " + corNome + item.nome + "");
                alvo->push_back("      " + corDesc + "  " + item.descricao + "");
                alvo->push_back("");
                count++;
            }
        }
        return count;
    };

    conquistasExibidas += processarCategoria("NPCs", CategoriaProgresso::NPC);
    conquistasExibidas += processarCategoria("Monstros", CategoriaProgresso::MONSTRO);
    conquistasExibidas += processarCategoria("Itens", CategoriaProgresso::ITEM);

    dados.totalConquistas = conquistasExibidas;
    return dados;
}

ItensCategorizados TelaDiarioLogic::categorizarItens(Character* jogador) {
    (void)jogador;
    ItensCategorizados resultado;
    std::vector<std::string> itens = Diary::instancia().obterItensDescobertos();

    for (const auto& itemNome : itens) {
        auto tempItem = ItemFactory::criarItem(itemNome);
        if (tempItem) {
            EquipmentType tipo = tempItem->getType();
            std::string prefixo;
            prefixo.reserve(3 + itemNome.size());
            prefixo = " - ";
            prefixo += itemNome;
            if (tipo == EquipmentType::Weapon) resultado.armas.push_back(prefixo);
            else if (tipo == EquipmentType::Shield) resultado.escudos.push_back(prefixo);
            else if (tipo == EquipmentType::Armor) resultado.armaduras.push_back(prefixo);
            else if (tipo == EquipmentType::Consumable) resultado.consumiveis.push_back(prefixo);
            else if (tipo == EquipmentType::Material) resultado.materiais.push_back(prefixo);
            else if (tipo == EquipmentType::Quest) resultado.missoes.push_back(prefixo);
            else resultado.outros.push_back(prefixo);
        } else {
            std::string prefixo;
            prefixo.reserve(3 + itemNome.size());
            prefixo = " - ";
            prefixo += itemNome;
            resultado.outros.push_back(prefixo);
        }
    }


    return resultado;
}

MissoesCategorizadas TelaDiarioLogic::categorizarMissoes(Character* jogador) {
    MissoesCategorizadas resultado;

    for (const auto& missao : registroDeMissoes) {
        if (Diary::instancia().missaoConcluida(missao.id)) {
            resultado.completas.push_back("[V] " + missao.nome);
        } else if (Diary::instancia().missaoAceita(missao.id)) {
            if (missao.checarRequisitos(jogador)) {
                resultado.prontas.push_back("[X] " + missao.nome);
            } else {
                resultado.emAndamento.push_back("[ ] " + missao.nome);
            }
        }
    }

    return resultado;
}

std::vector<GrupoCategorizado> TelaDiarioLogic::categorizarBestiario() {
    std::map<std::string, std::vector<std::string>> grupos;
    auto ordem = Bestiary::instancia().obterInimigosOrdenadosPorDificuldade();
    for (const auto& nome : ordem) {
        if (!Bestiary::instancia().estaDescoberto(nome)) continue;
        const auto* info = Bestiary::instancia().obterInfo(nome);
        std::string map = info ? info->map : "Desconhecido";
        grupos[map].push_back(nome);
    }

    std::vector<GrupoCategorizado> resultado;
    for (auto& par : grupos) {
        resultado.push_back({par.first, par.second});
    }
    return resultado;
}

std::vector<GrupoCategorizado> TelaDiarioLogic::categorizarNPCs() {
    std::map<std::string, std::vector<std::string>> grupos;
    auto npcs = Diary::instancia().obterNPCsDescobertos();
    for (const auto& nome : npcs) {
        std::string area = "Viajante";
        if (nome.find("Bjorn") != std::string::npos || nome.find("Cavaleiro Real") != std::string::npos) {
            area = "village/Kingdom";
        } else if (nome.find("Morgana") != std::string::npos) {
            area = "Forest";
        } else if (nome.find("Franchesco") != std::string::npos) {
            area = "Viajante";
        }
        grupos[area].push_back(nome);
    }

    std::vector<GrupoCategorizado> resultado;
    for (auto& par : grupos) {
        resultado.push_back({par.first, par.second});
    }
    return resultado;
}

std::vector<GrupoCategorizado> TelaDiarioLogic::categorizarRacas(const std::vector<std::string>& racasDescobertas) {
    std::vector<std::string> jogaveis, monstros;
    for (const auto& race : racasDescobertas) {
        if (race == "Human" || race == "Dwarf" || race == "Elf" || race == "Ork") {
            jogaveis.push_back(race);
        } else {
            monstros.push_back(race);
        }
    }

    std::vector<GrupoCategorizado> resultado;
    if (!jogaveis.empty()) resultado.push_back({"Races Jogaveis", jogaveis});
    if (!monstros.empty()) resultado.push_back({"Monstros e Enemies", monstros});
    return resultado;
}

std::vector<std::string> TelaDiarioLogic::obterTodasClasses() {
    return {"Warrior", "Mage", "Archer", "Bard", "Necromancer"};
}

std::vector<std::string> TelaDiarioLogic::quebrarTexto(const std::string& texto, int larguraMax) {
    std::vector<std::string> resultado;
    std::istringstream stream(texto);
    std::string linhaAtual;
    std::string palavra;
    while (stream >> palavra) {
        if (linhaAtual.length() + palavra.length() + (linhaAtual.empty() ? 0 : 1) > (size_t)larguraMax) {
            if (!linhaAtual.empty()) resultado.push_back(linhaAtual);
            linhaAtual = palavra;
        } else {
            if (!linhaAtual.empty()) linhaAtual += ' ';
            linhaAtual += palavra;
        }
    }
    if (!linhaAtual.empty()) resultado.push_back(linhaAtual);
    if (resultado.empty() && !texto.empty()) resultado.push_back(texto);
    return resultado;
}
