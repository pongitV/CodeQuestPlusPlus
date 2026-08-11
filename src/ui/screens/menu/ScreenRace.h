#pragma once
#include <string>
#include "../../../entities/races/RaceBase.h"

class TelaRaca {
public:
    struct Resultado {
        int indice = 0;
        bool voltou = false;
        std::string nome;
        RaceType racaSelecionada = RaceType::Nenhum;
    };
    static Resultado display(const std::string& nomeJogador);
};
