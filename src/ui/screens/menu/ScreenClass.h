#pragma once
#include <string>
#include "../../../entities/classes/ClassBase.h"

class TelaClasse {
public:
    struct Resultado {
        int indice = 0;
        bool voltou = false;
        std::string nome;
        ClassType classeSelecionada = ClassType::Nenhum;
    };
    static Resultado display(const std::string& nomeJogador, const std::string& nomeRaca);
};
