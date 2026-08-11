#pragma once

#include <string>
#include <vector>

#include "../../../entities/character/Character.h"

class TelaMenu {
public:
    static void displayPainelLogoJogo(const std::string& tituloDaTela = "", bool animarFadeIn = false);
    static bool displayConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);
    static std::vector<std::string> comporQuadroDeAtributos(const Attributes& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab, const std::string& tituloHabilidade2 = "", const std::string& nomeHab2 = "", const std::string& descHab2 = "");
    static int displayOpcoesMenuPrincipal();
    static void displayTutorialDeParry(const std::string& infoBox = "");

    // English Aliases
    static void showGameLogoPanel(const std::string& title = "", bool fadeIn = false) { displayPainelLogoJogo(title, fadeIn); }
    static int showMainMenuOptions() { return displayOpcoesMenuPrincipal(); }
    static void showParryTutorial(const std::string& infoBox = "") { displayTutorialDeParry(infoBox); }
};

using MenuScreen = TelaMenu;
