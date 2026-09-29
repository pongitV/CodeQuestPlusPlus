#include "Elf.h"

#include <iostream>

#include "../../../core/utils/RandomGenerator.h"
#include "../../../ui/screens/combat/ScreenCombat.h"

// Informacoes da raca
std::string Elf::getRaceName() const 
{
    return "Elf";
}

Attributes Elf::getRaceAttributes() const
{
    return { 90, 5, 15, 0, 10, 15, 5 };
}

// Aparencia
const std::vector<std::string>& Elf::getRaceAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

// Habilidade da raca
std::string Elf::getRaceAbilityName() const 
{ 
    return "Agil e preciso"; 
}

std::string Elf::getRaceAbilityDescription() const 
{ 
    return "Possui 33% chance de causar 1.5x de damage em cada ataque"; 
}

// Processamento de dano
int Elf::processOffensiveDamage(int baseDamage, Character* attacker) 
{
    if (RandomGenerator::rollChance(33)) 
    {
        std::string msg = TelaCombate::margemCombate() + "[PASSIVA]: Agil e preciso! Golpe critico.\n";
        TelaCombate::adicionarMensagemFixa(msg);
        return static_cast<int>(baseDamage * 1.5);
    }
    return baseDamage;
}






