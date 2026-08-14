#pragma once

#include "../../entities/character/Character.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include "../interfaces/MapInteraction.h"

class MapControllera 
{
public:
    // Processa o input de movimento e de menus. 
    // processarInputEComandos movido para MapInputControllera.h

    static void processarCombate(
        Character* currentPlayer, std::vector<std::string>& matrizDoMapaAtual, 
        int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
        const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<Character>> inimigosParaBatalha, 
        int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int amountDeCelulasOcupadas, int larguraDoTerminal, const std::function<void()>& restaurarTela);

    // animarIntroducaoMapa e animarFlashbang movidos para MapAnimatora.h

    // Funções de câmera e renderização movidas para RenderizadorMapa.h

    static bool isExploracao3DAtiva();
    static void sinalizarTrocaDeMapa3D();
    static float obterPosCamera3DX();
    static float obterPosCamera3DY();
    static float obterAnguloCamera3D();
    static std::string obterTituloMapaAtual();
    static std::vector<std::string> obterMatrizDoMapaAtual();

    static std::string formatarCelula(char celula, int x, int y, const std::string& tituloDoMapa, const std::vector<std::string>& matrizDoMapa, bool isMinimapa = false);

    static ProximaTransicaoMapa executarLoopDeExploracao(
        Character* currentPlayer,
        std::vector<std::string>& matrizDoMapaAtual,
        int& posicaoXDoJogador,
        int& posicaoYDoJogador,
        bool& exploracaoEstaAtiva,
        const std::string& tituloDoMapaAtual,
        const std::function<std::string()>& obterSimbolosInimigos,
        const std::function<std::vector<std::string>()>& obterLayoutOriginal,
        const std::function<void(int, int, int)>& processarInteracao,
        const std::function<std::string(char, int, int)>& formatador,
        const std::function<void()>& restaurarTela,
        int& linhaInicialParaDesenharOMapa,
        bool& precisaRenderizar
    );

    // English Aliases
    static bool is3DExplorationActive() { return isExploracao3DAtiva(); }
    static void signal3DMapChange() { sinalizarTrocaDeMapa3D(); }
    static std::string getCurrentMapTitle() { return obterTituloMapaAtual(); }
};

using MapControl = MapControllera;
using MapController = MapControllera;
