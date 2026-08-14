#pragma once
#include <vector>
#include <string>
#include <functional>

class MapLoader {
public:
    static void enterSubMap(
        std::vector<std::string>& currentMapMatrix, std::vector<std::string>& savedMainMapMatrix,
        int& savedPosXBeforeSubMap, int& savedPosYBeforeSubMap,
        int& playerPosX, int& playerPosY, bool& isInsideSubMap,
        std::string& currentMapTitle, std::vector<std::string>& savedSubMapMatrix, bool& subMapVisited,
        const std::vector<std::string>& generatedSubMapMatrix, int initialSubMapPosX, int initialSubMapPosY, const std::string& subMapTitle, const std::function<void()>& restoreScreen);

    static void standardizeMapSize(std::vector<std::string>& mapMatrix);

    // Aliases legados
    static void entrarSubMapa(
        std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
        int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
        std::string& tituloDoMapaAtual, std::vector<std::string>& matrizDoSubMapaSalva, bool& subMapaJaFoiVisitado,
        const std::vector<std::string>& matrizDoSubMapaGerada, int posicaoXInicialNoSubMapa, int posicaoYInicialNoSubMapa, const std::string& tituloDoSubMapa, const std::function<void()>& restaurarTela) {
        enterSubMap(matrizDoMapaAtual, matrizDoMapaPrincipalSalva, posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa, posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa, tituloDoMapaAtual, matrizDoSubMapaSalva, subMapaJaFoiVisitado, matrizDoSubMapaGerada, posicaoXInicialNoSubMapa, posicaoYInicialNoSubMapa, tituloDoSubMapa, restaurarTela);
    }

    static void padronizarTamanhoDoMapa(std::vector<std::string>& matrizDoMapa) {
        standardizeMapSize(matrizDoMapa);
    }
};

using MapLoadera = MapLoader;
