#include "Human.h"

#include <iostream>
#include "../../../ui/screens/combat/ScreenCombat.h"

// --- INFORMACOES DA RACA ---
std::string Human::getRaceName() const 
{
    return "Human";
}

Attributes Human::getRaceAttributes() const
{
    return { 100, 10, 10, 0, 10, 10, 10 };
}

// --- APARENCIA ---
const std::vector<std::string>& Human::getRaceAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

// --- HABILIDADE DA RACA ---
std::string Human::getRaceAbilityName() const 
{ 
    return "Espirito indomavel"; 
}

std::string Human::getRaceAbilityDescription() const 
{ 
    return "Revive com metade da health maxima uma vez"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Human::processDefensiveDamage(int finalDamage, Character* defensor) 
{
    // Verifica se o golpe seria fatal
    if ((defensor->obterVida() - finalDamage) <= 0 && defensor->podeUsarRessurreicao()) 
    {
        defensor->consumirRessurreicao();
        int curaReviver = defensor->obterVidaMaxima() / 2;
        defensor->modificarVida(curaReviver);
        std::string msg = TelaCombate::margemCombate() + std::string("[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua health maxima!") + "\n";
        TelaCombate::adicionarMensagemFixa(msg);
        return 0; // O damage atual e anulado pois a health foi resetada
    }
    return finalDamage;
}






