#include "ScreenManager.h"
#include "./screens/menu/ScreenOpeningRaycaster.h"
#include "./screens/menu/ScreenMenuRaycaster.h"
#include "./screens/menu/ScreenNameRaycaster.h"
#include "./screens/menu/ScreenRaceRaycaster.h"
#include "./screens/menu/ScreenClassRaycaster.h"
#include "./screens/menu/ScreenDifficultyRaycaster.h"
#include "./screens/menu/ScreenParryRaycaster.h"
#include "./screens/menu/ScreenIntroductionRaycaster.h"
#include "../../ui/screens/menu/ScreenTutorial.h"

void GerenciadorTelasRaycaster::abertura() {
    TelaAberturaRaycaster::display();
}

void GerenciadorTelasRaycaster::painelLogo(const std::string& tituloDaTela, bool animarFadeIn) {
    TelaMenuRaycaster::displayPainelLogoJogo(tituloDaTela, animarFadeIn);
}

bool GerenciadorTelasRaycaster::confirmacaoEscolha(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
    const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) {
    return TelaMenuRaycaster::displayConfirmacaoDeEscolhaComArteLadoALado(tipoDeEscolha, nomeDaEscolha, informacoesParaExibir, arteAsciiParaExibir);
}

std::vector<std::string> GerenciadorTelasRaycaster::quadroAtributos(const Attributes& stats,
    const std::string& tituloSecao, const std::string& tituloHabilidade,
    const std::string& nomeHab, const std::string& descHab,
    const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    return TelaMenuRaycaster::comporQuadroDeAtributos(stats, tituloSecao, tituloHabilidade, nomeHab, descHab, tituloHabilidade2, nomeHab2, descHab2);
}

int GerenciadorTelasRaycaster::menuPrincipal() {
    return TelaMenuRaycaster::displayOpcoesMenuPrincipal();
}

void GerenciadorTelasRaycaster::tutorialParry(const std::string& infoBox) {
    TelaTutorial::displayTutorialDeParry(infoBox); // Tutorial nao e separado ainda
}

TelaNome::Resultado GerenciadorTelasRaycaster::telaNome() {
    return TelaNomeRaycaster::display();
}

TelaRaca::Resultado GerenciadorTelasRaycaster::telaRaca(const std::string& nomePersonagem) {
    return TelaRacaRaycaster::display(nomePersonagem);
}

TelaClasse::Resultado GerenciadorTelasRaycaster::telaClasse(const std::string& nomePersonagem, const std::string& race) {
    return TelaClasseRaycaster::display(nomePersonagem, race);
}

TelaDificuldade::Resultado GerenciadorTelasRaycaster::telaDificuldade(const std::string& nomePersonagem, const std::string& race, const std::string& classe) {
    return TelaDificuldadeRaycaster::display(nomePersonagem, race, classe);
}

TelaParry::Resultado GerenciadorTelasRaycaster::telaParry(const std::string& nomePersonagem, const std::string& race, const std::string& classe) {
    return TelaParryRaycaster::display(nomePersonagem, race, classe);
}

void GerenciadorTelasRaycaster::telaIntroducao() {
    TelaIntroducaoRaycaster::display();
}

bool GerenciadorTelasRaycaster::confirmarSaida() {
    return TelaMenuRaycaster::displayConfirmacaoSaida();
}
