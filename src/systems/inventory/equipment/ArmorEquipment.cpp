#include "ArmorEquipment.h"
#include "../../../entities/character/Character.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"

EquipamentoArmadura::EquipamentoArmadura(const std::string& nome, int reducaoFixa, int reqResistencia, int reqConstituicao, int price) 
    : BaseEquipment(price), nome(nome), reducaoFixa(reducaoFixa), reqResistencia(reqResistencia), reqConstituicao(reqConstituicao), penalidadeDestreza(reducaoFixa / 3)
{
}

std::string EquipamentoArmadura::getNameItem() const { return nome; }
TipoEquipamento EquipamentoArmadura::obterTipo() const { return TipoEquipamento::ARMADURA; }

int EquipamentoArmadura::obterReducaoFixa() const { return reducaoFixa; }
int EquipamentoArmadura::obterReqResistencia() const { return reqResistencia; }
int EquipamentoArmadura::obterReqConstituicao() const { return reqConstituicao; }

bool Item::verificarRequisitosArmadura(Character* character, int reqResistencia, int reqConstituicao) {
    if (!character) return false;
    return character->getResistance() >= reqResistencia &&
           character->getConstitution() >= reqConstituicao;
}

bool EquipamentoArmadura::checarRequisitosEspecificos(Character* character) const {
    return Item::verificarRequisitosArmadura(character, reqResistencia, reqConstituicao);
}

bool EquipamentoArmadura::podeSerEquipadoPor(Character* character) const {
    return BaseEquipment::podeSerEquipadoPor(character);
}

std::vector<std::string> EquipamentoArmadura::obterDetalhesInspecao(Character* character) const {
    std::vector<std::string> linhas;
    linhas.push_back(" > Tipo: Armor");
    
    std::string defFixaStr = std::to_string(reducaoFixa) + " (Reduz damage recebido permanentemente)";
    if (character) {
        int defTotal = reducaoFixa + character->getResistance();
        defFixaStr += " -> C/ Seus Attributes: " + std::to_string(defTotal);
    }
    linhas.push_back(" > Defesa Fixa: " + defFixaStr);
    linhas.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqResistencia > 0) { linhas.push_back("   - Resistencia: " + std::to_string(reqResistencia)); hasReq = true; }
    if (reqConstituicao > 0) { linhas.push_back("   - Constituicao: " + std::to_string(reqConstituicao)); hasReq = true; }
    if (!hasReq) linhas.push_back("   - Nenhum requisito.");
    
    if (penalidadeDestreza > 0) {
        linhas.push_back(" > Penalidade: -" + std::to_string(penalidadeDestreza) + " Destreza");
    } else {
        linhas.push_back(" > Penalidade: Nenhuma");
    }
    
    if (temPropriedade(Propriedade::ArmaduraAdaptacao)) {
        linhas.push_back(" > Efeitos Ocultos: A Roda gira a cada turno regenerando 5% do HP e adapta a");
        linhas.push_back("                    sua defesa ao enemy e seu ataque a sua arma (+2 status)!");
    }
    return linhas;
}

std::string EquipamentoArmadura::obterInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reducaoFixa);
    if (penalidadeDestreza > 0) {
        info += " | -" + std::to_string(penalidadeDestreza) + " Dest";
    }
    
    std::string reqs = "";
    if (reqResistencia > 0 || reqConstituicao > 0) {
        reqs += " | Req: ";
        if (reqResistencia > 0) reqs += std::to_string(reqResistencia) + " Res ";
        if (reqConstituicao > 0) reqs += std::to_string(reqConstituicao) + " Con ";
    }

    return info + reqs + ")";
}

std::unique_ptr<Item> EquipamentoArmadura::gerarCopiaMelhorada() const {
    auto novaArmadura = std::make_unique<EquipamentoArmadura>(nome + "+", static_cast<int>(reducaoFixa * 1.5), reqResistencia, reqConstituicao, priceVenda * 2);
    for (Propriedade prop : propriedades) novaArmadura->adicionarPropriedade(prop);
    novaArmadura->adicionarPropriedade(Propriedade::Melhorado);
    return novaArmadura;
}

std::unique_ptr<Item> fabricarEquipamentoArmadura(ItemID id) {
    auto criarArmadura = [](ItemID id, int def, int rRes, int rCon, int price) {
        return std::make_unique<EquipamentoArmadura>(ItemFactory::getNameDeID(id), def, rRes, rCon, price);
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::ArmaduraMalha, [criarArmadura]() { return criarArmadura(ItemID::ArmaduraMalha, 7, 0, 0, 3); }},
        {ItemID::ArmaduraCouro, [criarArmadura]() { return criarArmadura(ItemID::ArmaduraCouro, 5, 0, 0, 3); }},
        {ItemID::Tunica, [criarArmadura]() { return criarArmadura(ItemID::Tunica, 2, 0, 0, 3); }},
        {ItemID::TrajeNobre, [criarArmadura]() { return criarArmadura(ItemID::TrajeNobre, 4, 0, 0, 3); }},
        {ItemID::ArmaduraTrapos, [criarArmadura]() { return criarArmadura(ItemID::ArmaduraTrapos, 3, 0, 0, 3); }},
        {ItemID::ArmaduraCavaleiro, [criarArmadura]() { return criarArmadura(ItemID::ArmaduraCavaleiro, 12, 0, 0, 0); }},
        {ItemID::RoupasRitualista, [criarArmadura]() { return criarArmadura(ItemID::RoupasRitualista, 3, 0, 0, 15); }},
        {ItemID::ArmaduraBau, [criarArmadura]() { 
            auto armadura = criarArmadura(ItemID::ArmaduraBau, 20, 0, 0, 150); 
            armadura->definirPenalidadeDestreza(10);
            return armadura; 
        }},
        {ItemID::RodaAdaptacao, []() { 
            std::string nome = "Roda da Adaptacao";
            auto armadura = std::make_unique<EquipamentoArmadura>(nome, 40, 0, 0, 5000); 
            armadura->definirPenalidadeDestreza(0);
            armadura->adicionarPropriedade(Propriedade::ArmaduraAdaptacao);
            return armadura; 
        }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
