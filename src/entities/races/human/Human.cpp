#include "Human.h"

#include <iostream>
#include "../../../ui/screens/combat/ScreenCombat.h"

// --- INFORMAÇÕES DA RAÇA ---
std::string Human::getRaceName() const 
{
    return "Human";
}

Attributes Human::getRaceAttributes() const
{
    return { 100, 10, 10, 0, 10, 10, 10 };
}

// --- APARÊNCIA ---
const std::vector<std::string>& Human::getRaceAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

// --- HABILIDADE DA RAÇA ---
std::string Human::getRaceAbilityName() const 
{ 
    return "Espirito indomavel"; 
}

std::string Human::getRaceAbilityDescription() const 
{ 
    return "Revive com metade da health maxima uma vez"; 
}

// --- PROCESSAMENTO DE DANO ---
int Human::processDefensiveDamage(int finalDamage, Character* defender) 
{
    // Verifica se o golpe seria fatal
    if ((defender->obterVida() - finalDamage) <= 0 && defender->podeUsarRessurreicao()) 
    {
        defender->consumirRessurreicao();
        int reviveHeal = defender->obterVidaMaxima() / 2;
        defender->modificarVida(reviveHeal);
        std::string msg = TelaCombate::margemCombate() + std::string("[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua health maxima!") + "\n";
        TelaCombate::adicionarMensagemFixa(msg);
        return 0; // O dano atual é anulado pois a vida foi restaurada
    }
    return finalDamage;
}






