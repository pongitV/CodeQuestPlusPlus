#include "WeaponEquipment.h"

#include <iostream>
#include <vector>

#include "../../../entities/character/Character.h"
#include "../../../core/utils/RandomGenerator.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"
#include "../../../core/utils/Color.h"

bool Item::verificarRequisitosAtributos(Character* character, int reqForca, int reqDestreza, int reqInteligencia, int reqSabedoria) {
    if (!character) return false;
    return character->getStrength() >= reqForca &&
           character->getDexterity() >= reqDestreza &&
           character->getInteligencia() >= reqInteligencia &&
           character->getWisdom() >= reqSabedoria;
}

EquipamentoArma::EquipamentoArma(const std::string& nome, int danoFisico, int danoMagico, int reqForca, int reqDestreza, int reqInteligencia, int reqSabedoria, int price)
    : BaseEquipment(price), nome(nome), danoFisico(danoFisico), danoMagico(danoMagico), reqForca(reqForca), reqDestreza(reqDestreza), reqInteligencia(reqInteligencia), reqSabedoria(reqSabedoria), efeitoSangramento(false), efeitoLentidao(false)
{
}

std::string EquipamentoArma::getNameItem() const { return nome; }
void EquipamentoArma::alterarNome(const std::string& n) { nome = n; }
TipoEquipamento EquipamentoArma::obterTipo() const { return TipoEquipamento::ARMA; }

int EquipamentoArma::obterDanoFisico() const { return danoFisico; }
int EquipamentoArma::obterDanoMagico() const { return danoMagico; }

int EquipamentoArma::obterReqForca() const { return reqForca; }
int EquipamentoArma::obterReqDestreza() const { return reqDestreza; }
int EquipamentoArma::obterReqInteligencia() const { return reqInteligencia; }
int EquipamentoArma::obterReqSabedoria() const { return reqSabedoria; }

bool EquipamentoArma::possuiEfeitoSangramento() const { return efeitoSangramento; }
bool EquipamentoArma::possuiEfeitoLentidao() const { return efeitoLentidao; }

bool EquipamentoArma::checarRequisitosEspecificos(Character* character) const {
    return Item::verificarRequisitosAtributos(character, reqForca, reqDestreza, reqInteligencia, reqSabedoria);
}

bool EquipamentoArma::podeSerEquipadoPor(Character* character) const {
    return BaseEquipment::podeSerEquipadoPor(character);
}

std::vector<std::string> EquipamentoArma::obterDetalhesInspecao(Character* character) const {
    std::vector<std::string> linhas;
    linhas.push_back(" > Tipo: Weapon");

    std::string fisStr = std::to_string(danoFisico);
    std::string magStr = std::to_string(danoMagico);

    if (character) {
        int strength = character->getStrength();
        int dexterity = character->getDexterity();
        int inteli = character->getInteligencia();
        int wisdom = character->getWisdom();
        
        if (danoFisico == 0 && danoMagico > 0) { strength /= 10; dexterity /= 10; }
        else if (danoFisico > 0 && danoMagico == 0) { inteli /= 10; wisdom /= 10; }
        
        int danoFisEst = std::max(0, static_cast<int>((danoFisico + strength) * (1.0 + (dexterity / 100.0)) * character->obterMultiplicador()));
        int danoMagEst = std::max(0, static_cast<int>((danoMagico + inteli) * (1.0 + (wisdom / 100.0)) * character->obterMultiplicador()));
        
        fisStr += " -> C/ Seus Attributes: " + std::to_string(danoFisEst);
        magStr += " -> C/ Seus Attributes: " + std::to_string(danoMagEst);
    }

    linhas.push_back(" > Damage Fisico: " + fisStr);
    linhas.push_back(" > Damage Magico: " + magStr);
    linhas.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqForca > 0) { linhas.push_back("   - Forca: " + std::to_string(reqForca)); hasReq = true; }
    if (reqDestreza > 0) { linhas.push_back("   - Destreza: " + std::to_string(reqDestreza)); hasReq = true; }
    if (reqInteligencia > 0) { linhas.push_back("   - Inteligencia: " + std::to_string(reqInteligencia)); hasReq = true; }
    if (reqSabedoria > 0) { linhas.push_back("   - Sabedoria: " + std::to_string(reqSabedoria)); hasReq = true; }
    if (!hasReq) linhas.push_back("   - Nenhum requisito.");
    
    linhas.push_back(" > Efeitos e Propriedades:");
    bool hasEfeito = false;
    if (efeitoSangramento) { linhas.push_back("   - Sangramento (Damage continuo no alvo)"); hasEfeito = true; }
    if (efeitoLentidao) { linhas.push_back("   - Lentidao (Reduz dexterity do alvo)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::Penetrante)) { linhas.push_back("   - Penetrante (Reduz resistance do alvo)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::Magica)) { linhas.push_back("   - Magica (Parte do damage ignora defesa)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::IgnoraDefesa)) { linhas.push_back("   - Exterminio (Ignora 100% da Resistencia e Constituicao do alvo)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::ViolaoMagico)) { linhas.push_back("   - Raizes Drenantes (Causa damage e cura o usuario)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::CipoPrisao)) { linhas.push_back("   - Prisao de Cipos (Chance de atordoar alvo)"); hasEfeito = true; }
    if (!hasEfeito) linhas.push_back("   - Nenhuma propriedade extra.");
    return linhas;
}

std::string EquipamentoArma::obterInfoStatus() const {
    std::string ef = "";
    if (possuiEfeitoSangramento()) ef += " | +Sangramento";
    if (possuiEfeitoLentidao()) ef += " | +Lentidao";
    if (temPropriedade(Propriedade::Penetrante)) ef += " | +Penetracao";
    
    std::string reqs = "";
    bool hasReq = false;
    if (reqForca > 0) { reqs += std::to_string(reqForca) + " For "; hasReq = true; }
    if (reqDestreza > 0) { reqs += std::to_string(reqDestreza) + " Des "; hasReq = true; }
    if (reqInteligencia > 0) { reqs += std::to_string(reqInteligencia) + " Int "; hasReq = true; }
    if (reqSabedoria > 0) { reqs += std::to_string(reqSabedoria) + " Sab "; hasReq = true; }
    if (hasReq) reqs = " | Req: " + reqs;

    return " (Damage: " + std::to_string(danoFisico) + "F/" + std::to_string(danoMagico) + "M" + ef + reqs + ")";
}

void EquipamentoArma::aplicarEfeitoSangramento() { efeitoSangramento = true; }
void EquipamentoArma::aplicarEfeitoLentidao() { efeitoLentidao = true; }

void EquipamentoArma::antesDeCausarDano(Character* atacante, Character* alvo) {
    if (temPropriedade(Propriedade::Penetrante) && !alvo->possuiEfeito(EfeitoID::QuebraResistencia)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoQuebraResistencia>());
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> A arma de " + atacante->getName() + " ativou o po magico! O ataque enfraqueceu " + alvo->getName() + " ate o fim do combat!\n");
    }
}

void EquipamentoArma::aoCausarDano(Character* atacante, Character* alvo, int danoCausado) {
    if (danoCausado <= 0) return;

    if (temPropriedade(Propriedade::ViolaoMagico) && !alvo->possuiEfeito(EfeitoID::SugaSangue)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoSugaSangue>(2, atacante));
    }

    if (temPropriedade(Propriedade::CipoPrisao) && RandomGenerator::rolarChance(30) && !alvo->possuiEfeito(EfeitoID::Atordoamento)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoAtordoamento>(1));
    }

    if (possuiEfeitoSangramento() && !alvo->possuiEfeito(EfeitoID::Sangramento)) {
        int danoSangramento = std::max(1, alvo->obterVidaMaxima() / 10);
        alvo->adicionarEfeito(std::make_unique<EfeitoSangramento>(3, danoSangramento));
        Color corSangramento = (alvo->getNameClasse() != "Monstro") ? Color::RED_CLARO : Color::RED;
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> " + alvo->getName() + " comecou a sangrar profundamente! (3 turnos)\n");
    }

    if (possuiEfeitoLentidao() && !alvo->possuiEfeito(EfeitoID::Lentidao)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> " + alvo->getName() + " foi coberto por gosma e sua dexterity caiu pela metade! (3 turnos)\n");
    }
}

int EquipamentoArma::garantirDanoMinimo(int finalDamage) {
    int minimo = 1;
    if (temPropriedade(Propriedade::ViolaoBase)) {
        minimo = std::max(minimo, danoMagico);
    }
    return std::max(finalDamage, minimo);
}

std::unique_ptr<Item> EquipamentoArma::gerarCopiaMelhorada() const {
    auto novaArma = std::make_unique<EquipamentoArma>(nome + "+", static_cast<int>(danoFisico * 1.5), static_cast<int>(danoMagico * 1.5), reqForca, reqDestreza, reqInteligencia, reqSabedoria, priceVenda * 2);
    
    for (Propriedade prop : propriedades) novaArma->adicionarPropriedade(prop);
    novaArma->adicionarPropriedade(Propriedade::Melhorado);
    
    if (efeitoSangramento) novaArma->aplicarEfeitoSangramento();
    if (efeitoLentidao) novaArma->aplicarEfeitoLentidao();
    
    return novaArma;
}

std::unique_ptr<Item> fabricarEquipamentoArma(ItemID id) {
    auto criarArma = [](ItemID id, int dFis, int dMag, int rFor, int rDes, int rInt, int rSab, int price) {
        return std::make_unique<EquipamentoArma>(ItemFactory::getNameDeID(id), dFis, dMag, rFor, rDes, rInt, rSab, price);
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::AdagaPedra, [criarArma]() { return criarArma(ItemID::AdagaPedra, 5, 0, 0, 0, 0, 0, 3); }},
        {ItemID::ArcoMadeira, [criarArma]() { return criarArma(ItemID::ArcoMadeira, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::CajadoCristal, [criarArma]() { return criarArma(ItemID::CajadoCristal, 0, 30, 0, 0, 0, 0, 3); }},
        {ItemID::VarinhaCorroida, [criarArma]() { return criarArma(ItemID::VarinhaCorroida, 0, 25, 0, 0, 0, 0, 3); }},
        {ItemID::ViolaoEncantado, [criarArma]() { 
            auto violao = criarArma(ItemID::ViolaoEncantado, 0, 10, 0, 0, 0, 0, 3); 
            violao->adicionarPropriedade(Propriedade::ViolaoBase);
            return violao; 
        }},
        {ItemID::CajadoOsso, [criarArma]() { 
            auto arma = criarArma(ItemID::CajadoOsso, 2, 8, 0, 0, 5, 10, 10); 
            arma->adicionarPropriedade(Propriedade::Magica);
            return arma; 
        }},
        {ItemID::EspadaFerro, [criarArma]() { return criarArma(ItemID::EspadaFerro, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::MachadoGuerra, [criarArma]() { return criarArma(ItemID::MachadoGuerra, 15, 0, 10, 0, 0, 0, 3); }},
        {ItemID::GosmaAcidaArma, [criarArma]() { return criarArma(ItemID::GosmaAcidaArma, 2, 7, 0, 0, 0, 0, 3); }},
        {ItemID::TroncoAmarrotado, [criarArma]() { return criarArma(ItemID::TroncoAmarrotado, 40, 0, 25, 0, 0, 0, 30); }},
        {ItemID::EspadaCavaleiro, [criarArma]() { return criarArma(ItemID::EspadaCavaleiro, 12, 0, 0, 0, 0, 0, 0); }},
        {ItemID::EspadaExterminio, []() { 
            std::string nome = "Espada de Exterminio";
            auto arma = std::make_unique<EquipamentoArma>(nome, 65, 65, 40, 40, 0, 0, 5000);
            arma->adicionarPropriedade(Propriedade::Magica);
            arma->adicionarPropriedade(Propriedade::IgnoraDefesa);
            return arma;
        }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
