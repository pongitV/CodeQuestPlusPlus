#include "ScreenRace.h"
#include "../utils/ScreenRegistry.h"

TelaRaca::Resultado TelaRaca::display(const std::string& nomeJogador) {
    return RegistroTelas::telaRaca(nomeJogador);
}
