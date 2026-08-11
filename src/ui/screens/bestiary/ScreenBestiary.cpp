#include "ScreenBestiary.h"
#include "../../UIManager.h"

void TelaBestiario::displayLista(Character* currentPlayer) {
    GerenciadorPerspectiva::obterBestiarioUI().display({});
}

void TelaBestiario::displayFicha(Character* currentPlayer, const std::string& nomeInimigo, int indiceDescoberto, const std::vector<std::string>& descobertos) {
    GerenciadorPerspectiva::obterBestiarioUI().displayDetalhe(nullptr);
}
