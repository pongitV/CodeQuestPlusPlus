#include "Dwarf.h"

#include <iostream>

#include "../../../core/utils/RandomGenerator.h"
#include "../../../ui/screens/combat/ScreenCombat.h"

// Informacoes da raca
std::string Dwarf::getRaceName() const
{
    return "Dwarf";
}

Attributes Dwarf::getRaceAttributes() const
{
    return { 110, 15, 5, 0, 10, 5, 15 };
}

// Aparencia
const std::vector<std::string>& Dwarf::getRaceAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

// Habilidade da raca
std::string Dwarf::getRaceAbilityName() const 
{ 
    return "Forjado com determinacao"; 
}

std::string Dwarf::getRaceAbilityDescription() const 
{ 
    return "Escudos possuem o dobro de durabilidade"; 
}

// Processamento de dano
int Dwarf::processDefensiveDamage(int finalDamage, Character* defender) 
{
    if (defender->obterDefendendo() && defender->obterEscudo() != nullptr) 
    {
        if (RandomGenerator::rollChance(50)) 
        {
            defender->obterEscudo()->aumentarDurabilidade(1);
            std::string msg = TelaCombate::margemCombate() + "[PASSIVA]: Forjado com determinacao poupou a durabilidade do escudo!\n";
            TelaCombate::adicionarMensagemFixa(msg);
        }
    }
    return finalDamage;
}






