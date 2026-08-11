#pragma once
#include <string>

struct ResultadoDano {
    int finalDamage = 0;
    int danoBloqueado = 0;
    bool escudoQuebrou = false;
    std::string nomeEscudoQuebrado = "";
};
