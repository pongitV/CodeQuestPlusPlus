#include "Map3KingdomBridge.h"
#include "../utils/MapHelper.h"
#include "../../core/state/Debug.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

#include "../../core/state/GameMenu.h"
#include "../../ui/screens/menu/ScreenMenu.h"
#include "../control/MapController.h"
#include "../engine/MapAnimator.h"
#include "../engine/MapLoader.h"
#include "../../core/state/EnemyCreator.h"
#include "Map3KingdomBridgeLayout.h"
#include "../../entities/npcs/generic-knight/NPCGenericKnight.h"
#include "../../core/utils/Color.h"


Mapa3PonteReino::Mapa3PonteReino(Character* personagemJogador) :
    posicaoXDoJogador(47), 
    posicaoYDoJogador(32),
    currentPlayer(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("PONTE DO REINO"),
    proximoMapa(ProximaTransicaoMapa::Nenhuma)
{
    matrizDoMapaAtual = Mapa3PonteReinoLayouts::obterLayoutPonteReino();
    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaAtual);
}

Mapa3PonteReino::~Mapa3PonteReino() = default;

ProximaTransicaoMapa Mapa3PonteReino::iniciarLoopDeExploracao()
{
    bool trollDerrotado = false;
    bool conviteRecebido = false;

    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaAtual);


    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
            char ic = '@';
            if (ic <= 32 || ic > 126) ic = '@'; // Garante que o icone seja um caractere visivel
            return "" + std::string(1, ic) + "";
        }
        return MapControllera::formatarCelula(celula, x, y, tituloDoMapaAtual, matrizDoMapaAtual, false);
    };

    int linhaInicialParaDesenharOMapa = 0;

    auto restaurarTela = [&]() {
        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Color::CYAN, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false, true, nullptr);
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        std::vector<std::string> arteTrans;
        int larguraTrans = 0;

        if (tituloDoMapaAtual == "PONTE DO REINO" || tituloDoMapaAtual == "CAMINHO DO Kingdom") {
            arteTitulo = Mapa3PonteReinoLayouts::obterLogoPonteReino();
            larguraArte = 150; // A nova arte ASCII tem cerca de 150 caracteres de largura
            arteTrans = Mapa3PonteReinoLayouts::obterArteTransicaoPonteReino();
            larguraTrans = 75;
        }

        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Color::CYAN, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, true, nullptr);
    };

    auto animarTela_ = animarTela; // Para fins estáticos
    animarTela();

    std::unordered_map<char, std::function<void(int, int, int)>> interacoes;

    interacoes['^'] = [&](int px, int py, int larg) {
        // 1. Acesso ao Kingdom
        if (py < 20) {
            if (!conviteRecebido) {
                std::vector<std::string> msg = { "Os portoes estao trancados.", "Voce precisa de uma permissao real." };
            } else {
                std::vector<std::string> msg = {
                    "Voce apresentou o Convite Real e os portoes se abriram!",
                    "Entrando no Kingdom do Kingdom..."
                };
                exploracaoEstaAtiva = false;
                proximoMapa = ProximaTransicaoMapa::Kingdom;
            }
        }
        // 2. Retornar para a Forest
        else if (py >= 20) {
            exploracaoEstaAtiva = false;
            proximoMapa = ProximaTransicaoMapa::Forest;
        }
    };

    interacoes['G'] = [&](int px, int py, int larg) {
        std::vector<std::string> msg = {
            "Alto la! Somente o Rei pode conceder passagem.",
            "(O Kingdom ainda esta em construcao pelos devs)"
        };
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    };

    auto interagirCavaleiro = [&](int px, int py, int larg) {
        NPCGenericKnight::interagir(currentPlayer, trollDerrotado, conviteRecebido, larg, matrizDoMapaAtual, exploracaoEstaAtiva, restaurarTela, matrizDoMapaAtual[py][px], px, py);
    };
    interacoes['T'] = interagirCavaleiro;
    interacoes['C'] = interagirCavaleiro;

    auto processarInteracao = [&](int px, int py, int larg) {
        char celulaDestino = matrizDoMapaAtual[py][px];
        auto it = interacoes.find(celulaDestino);
        if (it != interacoes.end()) {
            it->second(px, py, larg);
        } else {
            MapHelper::processarMovimento(posicaoXDoJogador, posicaoYDoJogador, px, py, celulaDestino, "*=|[]ASELO ");
        }
    };

    bool precisaRenderizar = true;
    ProximaTransicaoMapa destinoViagemRapida = MapControllera::executarLoopDeExploracao(
        currentPlayer, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual, []() { return ""; },
        []() -> std::vector<std::string> { return {}; },
        processarInteracao, formatador, restaurarTela,
        linhaInicialParaDesenharOMapa, precisaRenderizar
    );

    if (destinoViagemRapida != ProximaTransicaoMapa::Nenhuma) {
        return destinoViagemRapida;
    }

    if (currentPlayer->obterVida() <= 0 || currentPlayer->obterVoltarProMenu()) {
        return ProximaTransicaoMapa::VoltarMenu;
    }
    return proximoMapa;
}
