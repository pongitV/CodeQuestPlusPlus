#include "ShieldEquipment.h"
#include <memory>
#include "../../../entities/character/Character.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../ItemFactory.h"

EquipamentoEscudo::EquipamentoEscudo(const std::string& nome, int reducaoFixa, int durabilidade, int reqResistencia, int reqSecundario, TipoAtributo tipoSecundario, int price)
    : BaseEquipment(price), nome(nome), reducaoFixa(reducaoFixa), durabilidade(durabilidade), durabilidadeMaxima(durabilidade), reqResistencia(reqResistencia), reqSecundario(reqSecundario), tipoSecundario(tipoSecundario)
{
}

std::string EquipamentoEscudo::getNameItem() const { return nome; }
TipoEquipamento EquipamentoEscudo::obterTipo() const { return TipoEquipamento::ESCUDO; }

int EquipamentoEscudo::obterDurabilidadeMaxima() const { return durabilidadeMaxima; }
void EquipamentoEscudo::definirDurabilidade(int novaDurabilidade) { durabilidade = novaDurabilidade; }
int EquipamentoEscudo::obterDurabilidadeAtualEscudo() const { return durabilidade; }
int EquipamentoEscudo::obterReducaoDanoFixaEscudo() const { return (durabilidade > 0) ? reducaoFixa : 0; }

int EquipamentoEscudo::obterReqResistencia() const { return reqResistencia; }
int EquipamentoEscudo::obterReqSecundario() const { return reqSecundario; }
TipoAtributo EquipamentoEscudo::obterTipoSecundario() const { return tipoSecundario; }

void EquipamentoEscudo::reduzirDurabilidade(int qtd) { 
    if (durabilidade <= 0) return; // Ja estava quebrado
    
    durabilidade -= qtd; 
    if (durabilidade <= 0) {
        durabilidade = 0;
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> O escudo [" + nome + "] quebrou e perdeu seu poder de bloqueio!\n");
    }
}
void EquipamentoEscudo::aumentarDurabilidade(int qtd) { durabilidade += qtd; }

bool Item::verificarRequisitosEscudo(Character* character, int reqResistencia, int secVal, int reqSecundario) {
    if (!character) return false;
    return character->getResistance() >= reqResistencia && secVal >= reqSecundario;
}

bool EquipamentoEscudo::checarRequisitosEspecificos(Character* character) const {
    int secVal = 0;
    switch(tipoSecundario) {
        case TipoAtributo::Forca: secVal = character->getStrength(); break;
        case TipoAtributo::Destreza: secVal = character->getDexterity(); break;
        case TipoAtributo::Inteligencia: secVal = character->getInteligencia(); break;
        case TipoAtributo::Sabedoria: secVal = character->getWisdom(); break;
        default: secVal = 9999;
    }
    return Item::verificarRequisitosEscudo(character, reqResistencia, secVal, reqSecundario);
}

bool EquipamentoEscudo::podeSerEquipadoPor(Character* character) const {
    return BaseEquipment::podeSerEquipadoPor(character);
}

std::vector<std::string> EquipamentoEscudo::obterDetalhesInspecao(Character* character) const {
    std::vector<std::string> linhas;
    linhas.push_back(" > Tipo: Shield");

    std::string bloqueioStr = std::to_string(reducaoFixa) + " (Damage bloqueado na acao 'Defender')";
    if (character) {
        int defTotal = reducaoFixa + character->getResistance();
        bloqueioStr += " -> C/ Seus Attributes: " + std::to_string(defTotal);
    }
    linhas.push_back(" > Poder de Bloqueio: " + bloqueioStr);
    linhas.push_back(" > Durabilidade Maxima: " + std::to_string(durabilidade) + " usos");
    linhas.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqResistencia > 0) { linhas.push_back("   - Resistencia Base: " + std::to_string(reqResistencia)); hasReq = true; }
    if (reqSecundario > 0) {
        std::string atrSec = "";
        if (tipoSecundario == TipoAtributo::Forca) atrSec = "Forca";
        else if (tipoSecundario == TipoAtributo::Destreza) atrSec = "Destreza";
        else if (tipoSecundario == TipoAtributo::Inteligencia) atrSec = "Inteligencia";
        else if (tipoSecundario == TipoAtributo::Sabedoria) atrSec = "Sabedoria";
        linhas.push_back("   - Atributo Secundario (" + atrSec + "): " + std::to_string(reqSecundario));
        hasReq = true;
    }
    if (!hasReq) linhas.push_back("   - Nenhum requisito.");
    return linhas;
}

std::string EquipamentoEscudo::obterInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reducaoFixa) + " | Dur: " + std::to_string(durabilidade) + "/" + std::to_string(durabilidadeMaxima);
    std::string reqs = "";
    if (reqResistencia > 0 || reqSecundario > 0) {
        reqs += " | Req: ";
        if (reqResistencia > 0) reqs += std::to_string(reqResistencia) + " Res ";
        if (reqSecundario > 0) {
            reqs += std::to_string(reqSecundario) + " ";
            if (tipoSecundario == TipoAtributo::Forca) reqs += "For ";
            else if (tipoSecundario == TipoAtributo::Destreza) reqs += "Des ";
            else if (tipoSecundario == TipoAtributo::Inteligencia) reqs += "Int ";
            else if (tipoSecundario == TipoAtributo::Sabedoria) reqs += "Sab ";
        }
    }
    
    std::string tag = "";
    if (durabilidade <= 0) {
        tag = " [QUEBRADO]";
    } else if (durabilidade < durabilidadeMaxima) {
        tag = " [D]";
    }
    return info + reqs + ")" + tag;
}

std::unique_ptr<Item> EquipamentoEscudo::gerarCopiaMelhorada() const {
    auto novoEscudo = std::make_unique<EquipamentoEscudo>(nome + "+", static_cast<int>(reducaoFixa * 1.5), static_cast<int>(durabilidadeMaxima * 1.5), reqResistencia, reqSecundario, tipoSecundario, priceVenda * 2);
    for (Propriedade prop : propriedades) novoEscudo->adicionarPropriedade(prop);
    novoEscudo->adicionarPropriedade(Propriedade::Melhorado);
    return novoEscudo;
}

std::unique_ptr<Item> fabricarEquipamentoEscudo(ItemID id) {
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::EscudoMetal, []() { return std::make_unique<EquipamentoEscudo>(ItemFactory::getNameDeID(ItemID::EscudoMetal), 15, 5, 0, 0, TipoAtributo::Forca, 9); }},
        {ItemID::BarreiraMagica, []() { return std::make_unique<EquipamentoEscudo>(ItemFactory::getNameDeID(ItemID::BarreiraMagica), 50, 2, 0, 0, TipoAtributo::Inteligencia, 3); }},
        {ItemID::CapaMagica, []() { return std::make_unique<EquipamentoEscudo>(ItemFactory::getNameDeID(ItemID::CapaMagica), 6, 10, 0, 0, TipoAtributo::Sabedoria, 9); }},
        {ItemID::BracedeirasPrata, []() { return std::make_unique<EquipamentoEscudo>(ItemFactory::getNameDeID(ItemID::BracedeirasPrata), 5, 3, 0, 0, TipoAtributo::Destreza, 3); }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
