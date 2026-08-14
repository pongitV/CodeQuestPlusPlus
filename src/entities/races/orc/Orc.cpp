#include "Orc.h"

#include <iostream>
#include "../../../ui/screens/combat/ScreenCombat.h"

// --- INFORMAÇÕES DA RAÇA ---
std::string Orc::getRaceName() const 
{
    return "Ork";
}

Attributes Orc::getRaceAttributes() const
{
    return { 120, 20, 10, 0, 10, 5, 5 };
}

// --- APARÊNCIA ---
const std::vector<std::string>& Orc::getRaceAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

// --- HABILIDADE DA RAÇA ---
std::string Orc::getRaceAbilityName() const 
{ 
    return "Furia cega"; 
}

std::string Orc::getRaceAbilityDescription() const 
{ 
    return "Damage extra baseado na porcentagem de health perdida"; 
}

// --- PROCESSAMENTO DE DANO ---
int Orc::processOffensiveDamage(int baseDamage, Character* attacker) 
{
    double percHealthLost = 1.0 - (static_cast<double>(attacker->obterVida()) / attacker->obterVidaMaxima());
    int extraDamage = static_cast<int>(baseDamage * percHealthLost);
    if (extraDamage > 0) 
    {
        std::string msg = TelaCombate::margemCombate() + "[PASSIVA]: Furia cega aumentou o damage em " + std::to_string(extraDamage) + "!\n";
        TelaCombate::adicionarMensagemFixa(msg);
        return baseDamage + extraDamage;
    }
    return baseDamage;
}






