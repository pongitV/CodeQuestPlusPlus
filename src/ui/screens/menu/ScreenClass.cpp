#include "ScreenClass.h"
#include "../utils/ScreenRegistry.h"

TelaClasse::Resultado TelaClasse::display(const std::string& nomeJogador, const std::string& nomeRaca) {
    return RegistroTelas::telaClasse(nomeJogador, nomeRaca);
}
