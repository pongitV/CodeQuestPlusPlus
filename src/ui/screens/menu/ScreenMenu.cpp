#include "ScreenMenu.h"
#include "ScreenTutorial.h"
#include "../utils/ScreenRegistry.h"

void TelaMenu::displayPainelLogoJogo(const std::string& tituloDaTela, bool animarFadeIn) {
    RegistroTelas::painelLogo(tituloDaTela, animarFadeIn);
}

bool TelaMenu::displayConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) {
    return RegistroTelas::confirmacaoEscolha(tipoDeEscolha, nomeDaEscolha, informacoesParaExibir, arteAsciiParaExibir);
}

std::vector<std::string> TelaMenu::comporQuadroDeAtributos(const Attributes& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab, const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    return RegistroTelas::quadroAtributos(stats, tituloSecao, tituloHabilidade, nomeHab, descHab, tituloHabilidade2, nomeHab2, descHab2);
}

int TelaMenu::displayOpcoesMenuPrincipal() {
    return RegistroTelas::menuPrincipal();
}

void TelaMenu::displayTutorialDeParry(const std::string& infoBox) {
    TelaTutorial::displayTutorialDeParry(infoBox);
}
