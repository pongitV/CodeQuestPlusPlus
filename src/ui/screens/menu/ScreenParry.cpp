#include "ScreenParry.h"
#include "../utils/ScreenRegistry.h"

TelaParry::Resultado TelaParry::display(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    return RegistroTelas::telaParry(nomeJogador, nomeRaca, nomeClasse);
}
