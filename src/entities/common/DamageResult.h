#pragma once
#include <string>

struct DamageResult {
    int finalDamage = 0;
    int blockedDamage = 0;
    bool shieldBroke = false;
    std::string brokenShieldName = "";

    // Aliases para compatibilidade legada
    int& danoBloqueado = blockedDamage;
    bool& escudoQuebrou = shieldBroke;
    std::string& nomeEscudoQuebrado = brokenShieldName;
};

using ResultadoDano = DamageResult;
