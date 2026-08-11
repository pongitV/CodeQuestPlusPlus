#pragma once
#include <string>
#include "../../../../ui/screens/menu/ScreenDifficulty.h"

class TelaDificuldadeRaycaster {
public:
    static TelaDificuldade::Resultado display(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse);
};
