#include "Archer.h"

#include <iostream>
#include <memory>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"

// --- INFORMACOES DA CLASSE ---
std::string Archer::getNameClasse() const 
{
     return "Archer"; 
}

const std::vector<std::string>& Archer::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Archer::obterAtributosClasse() const
{
    return { 0, 10, 20, 3, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Archer::obterEquipamentoClasse() const 
{
    auto equipamentos = ItemFactory::criarKitPocoes();

    equipamentos.push_back(ItemFactory::criarItem(ItemID::ArcoMadeira));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::BracedeirasPrata));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::ArmaduraCouro));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Archer::getNamePassivaClasse() const 
{ 
    return "Passos leves"; 
}

std::string Archer::obterDescricaoPassivaClasse() const 
{ 
    return "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade."; 
}

int Archer::processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const 
{
    return penalidadeBase / 2;
}

int Archer::aplicarPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const 
{
    return (dexterityAtual * 3) / 4;
}

int Archer::reverterPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const 
{
    return (dexterityAtual * 4) / 3;
}

// --- HABILIDADE DA CLASSE ---
std::string Archer::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 1 turno."; 
}

std::string Archer::getNameHabilidadeClasse() const 
{ 
    return "Retirada com pontaria"; 
}

std::string Archer::getClassAbilityDescription() const 
{ 
    return "Se afasta durante um turno, no proximo turno causa 2x damage"; 
}

void Archer::useClassAbility(Combat* combat, Character* personagemUsuario, std::vector<Character*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterRecargaHabilidade(AbilityID::RetiradaComPontaria);
    if (verificarEReportarRecarga(personagemUsuario, turnosRestantes, getNameHabilidadeClasse())) return;

    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoInviolavel>(1));
    personagemUsuario->definirCooldown(AbilityID::RetiradaComPontaria, 2);
    
    std::string msg = DialogFunctions::formatarMsgHabilidade("Retirada com pontaria! (Afasta-se)");
    notificarMensagemCombate(msg, msg);
}
