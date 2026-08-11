#include "ScreenDifficulty.h"
#include "../utils/ScreenRegistry.h"

TelaDificuldade::Resultado TelaDificuldade::display(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    return RegistroTelas::telaDificuldade(nomeJogador, nomeRaca, nomeClasse);
}
