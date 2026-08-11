#include "Dwarf.h"

#include <iostream>

#include "../../../core/utils/RandomGenerator.h"
#include "../../../ui/screens/combat/ScreenCombat.h"

// --- INFORMACOES DA RACA ---
std::string Dwarf::getRaceName() const
{
    return "Dwarf";
}

Attributes Dwarf::getRaceAttributes() const
{
    return { 110, 15, 5, 0, 10, 5, 15 };
}

// --- APARENCIA ---
const std::vector<std::string>& Dwarf::getRaceAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

// --- HABILIDADE DA RACA ---
std::string Dwarf::getRaceAbilityName() const 
{ 
    return "Forjado com determinacao"; 
}

std::string Dwarf::getRaceAbilityDescription() const 
{ 
    return "Escudos possuem o dobro de durabilidade"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Dwarf::processDefensiveDamage(int finalDamage, Character* defensor) 
{
    if (defensor->obterDefendendo() && defensor->obterEscudo() != nullptr) 
    {
        if (RandomGenerator::rolarChance(50)) 
        {
            defensor->obterEscudo()->aumentarDurabilidade(1);
            std::string msg = TelaCombate::margemCombate() + "[PASSIVA]: Forjado com determinacao poupou a durabilidade do escudo!\n";
            TelaCombate::adicionarMensagemFixa(msg);
        }
    }
    return finalDamage;
}






