#pragma once
#include <string>
#include "../../../../ui/screens/menu/ScreenParry.h"

class TelaParryRaycaster {
public:
    static TelaParry::Resultado display(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse);
};
