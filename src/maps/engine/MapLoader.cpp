#include "MapLoader.h"
#include "../../ui/UIManager.h"
#include "../control/MapController.h"
void MapLoadera::entrarSubMapa(
    std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
    int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
    std::string& tituloDoMapaAtual, std::vector<std::string>& matrizDoSubMapaSalva, bool& subMapaJaFoiVisitado,
    const std::vector<std::string>& matrizDoSubMapaGerada, int posicaoXInicialNoSubMapa, int posicaoYInicialNoSubMapa, const std::string& tituloDoSubMapa, const std::function<void()>& restaurarTela)
{
    matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
    posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
    posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

    if (!subMapaJaFoiVisitado) { matrizDoMapaAtual = matrizDoSubMapaGerada; subMapaJaFoiVisitado = true; } 
    else { matrizDoMapaAtual = matrizDoSubMapaSalva; }
    padronizarTamanhoDoMapa(matrizDoMapaAtual);

    posicaoXDoJogador = posicaoXInicialNoSubMapa;
    posicaoYDoJogador = posicaoYInicialNoSubMapa;
    jogadorEstaDentroDeUmSubMapa = true;
    tituloDoMapaAtual = tituloDoSubMapa;
    if (!GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) restaurarTela();
    else MapControllera::sinalizarTrocaDeMapa3D();
}

void MapLoadera::padronizarTamanhoDoMapa(std::vector<std::string>& matrizDoMapa) {
    if (matrizDoMapa.empty()) return;
    size_t maxLen = 0;
    for (const auto& row : matrizDoMapa) {
        if (row.length() > maxLen) maxLen = row.length();
    }
    for (auto& row : matrizDoMapa) {
        if (row.length() < maxLen) {
            row.append(maxLen - row.length(), ' ');
        }
    }
}
