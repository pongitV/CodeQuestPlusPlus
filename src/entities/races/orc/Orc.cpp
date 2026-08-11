#include "Orc.h"

#include <iostream>
#include "../../../ui/screens/combat/ScreenCombat.h"

// --- INFORMACOES DA RACA ---
std::string Ork::getRaceName() const 
{
    return "Ork";
}

Attributes Ork::getRaceAttributes() const
{
    return { 120, 20, 10, 0, 10, 5, 5 };
}

// --- APARENCIA ---
const std::vector<std::string>& Ork::getRaceAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

// --- HABILIDADE DA RACA ---
std::string Ork::getRaceAbilityName() const 
{ 
    return "Furia cega"; 
}

std::string Ork::getRaceAbilityDescription() const 
{ 
    return "Damage extra baseado na porcentagem de health perdida"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Ork::processOffensiveDamage(int baseDamage, Character* atacante) 
{
    double percVidaPerdida = 1.0 - (static_cast<double>(atacante->obterVida()) / atacante->obterVidaMaxima());
    int danoExtra = static_cast<int>(baseDamage * percVidaPerdida);
    if (danoExtra > 0) 
    {
        std::string msg = TelaCombate::margemCombate() + "[PASSIVA]: Furia cega aumentou o damage em " + std::to_string(danoExtra) + "!\n";
        TelaCombate::adicionarMensagemFixa(msg);
        return baseDamage + danoExtra;
    }
    return baseDamage;
}






