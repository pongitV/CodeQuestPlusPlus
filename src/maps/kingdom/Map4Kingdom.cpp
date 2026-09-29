#include "Map4Kingdom.h"
#include "../utils/MapHelper.h"
#include "../../core/state/Debug.h"
#include "Map4KingdomLayout.h"
#include "../../core/state/GameMenu.h"
#include "../control/MapController.h"
#include "../engine/MapAnimator.h"
#include "../engine/MapLoader.h"
#include "../../core/utils/InputControl.h"
#include "../../entities/npcs/merchant/NPCMerchant.h"
#include "../../entities/npcs/appearance/NPCAppearance.h"
#include "../../entities/npcs/blacksmith/NPCBlacksmith.h"
#include "../../entities/npcs/generic-knight/NPCGenericKnight.h"
#include "../../entities/npcs/alchemist/NPCAlchemist.h"
#include "../../entities/npcs/priest/NPCPriest.h"
#include "../../systems/progress/Diary.h"
#include "../../systems/progress/Progression.h"
#include "../../systems/progress/ProgressionFlags.h"
#include "../../systems/combat/Combat.h"

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
#include "../../core/utils/Color.h"

Mapa4Reino::Mapa4Reino(Character* personagemJogador) :
    posicaoXDoJogador(41), 
    posicaoYDoJogador(41),
    currentPlayer(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("REINO"),
    proximoMapa(ProximaTransicaoMapa::Nenhuma),
    jogadorEstaDentroDeUmSubMapa(false),
    igrejaJaFoiVisitada(false)
{
    matrizDoMapaAtual = Mapa4ReinoLayouts::obterLayoutReino();
    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaAtual);
    matrizDoMapaPrincipalSalva = matrizDoMapaAtual; // Caso necessario
}

Mapa4Reino::~Mapa4Reino() = default;

ProximaTransicaoMapa Mapa4Reino::iniciarLoopDeExploracao()
{
    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
            char ic = '@';
            if (ic <= 32 || ic > 126) ic = '@'; // Garante caractere visivel
            return "" + std::string(1, ic) + "";
        }
        return MapControllera::formatarCelula(celula, x, y, tituloDoMapaAtual, matrizDoMapaAtual, false);
    };

    int linhaInicialParaDesenharOMapa = 0;

    auto restaurarTela = [&]() {
        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Color::MAGENTA, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false, true, nullptr);
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        
        // Garante que o titulo seja estritamente REINO para a faixa
        if (tituloDoMapaAtual == "REINO" || tituloDoMapaAtual.find("Kingdom") != std::string::npos || tituloDoMapaAtual.find("REINO") != std::string::npos) {
            tituloDoMapaAtual = "REINO";
            arteTitulo = Mapa4ReinoLayouts::obterLogoReino();
            larguraArte = 77;
        }
        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, {}, 0, Color::MAGENTA, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, true, nullptr);
    };

    animarTela();

    std::unordered_map<char, std::function<void(int, int, int)>> interacoes;

    // Triggers e Teleportes
    interacoes['^'] = [&](int px, int py, int larg) {
        if (jogadorEstaDentroDeUmSubMapa) {
            // Saindo da Igreja (submapa) de volta para o patio do Kingdom
            if (px == 18 && py == 3) {
                matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
                posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
                posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
                jogadorEstaDentroDeUmSubMapa = false;
                tituloDoMapaAtual = "REINO";
                restaurarTela();
            }
        } else {
            // Retornar para o Kingdom (Ponte)
            if (py > 30) {
                exploracaoEstaAtiva = false;
                proximoMapa = ProximaTransicaoMapa::Kingdom;
            }
            // Entrada do Palacio (agora no X=43, Y=1)
            else if (py == 1 && (px >= 40 && px <= 45)) {
                std::vector<std::string> msg = {
                    "Os grandes portoes do Palacio Real estao selados por runas magicas.",
                    "Uma barreira intransponivel impede sua passagem por enquanto.",
                    "A aventura continuara em breve..."
                };
                posicaoXDoJogador = px;
                posicaoYDoJogador = py + 1; // Recua um passo
                restaurarTela();
            }
        }
    };

    // Entrada da Igreja
    interacoes['I'] = [&](int px, int py, int larg) {
        if (!jogadorEstaDentroDeUmSubMapa) {
            MapLoadera::entrarSubMapa(
                matrizDoMapaAtual, matrizDoMapaPrincipalSalva,
                posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa,
                posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa,
                tituloDoMapaAtual, matrizDoMapaDaIgrejaSalva, igrejaJaFoiVisitada,
                Mapa4ReinoLayouts::obterLayoutIgreja(), 17, 3, "IGREJA DO REINO", restaurarTela
            );
        }
    };

    // Priest da Igreja (so funciona dentro do submapa da igreja)
    interacoes['P'] = [&](int px, int py, int larg) {
        if (jogadorEstaDentroDeUmSubMapa) {
            NPCPriest padre;
            padre.interagir(currentPlayer);
            Diary::instancia().registrarNPC("Priest Benedito");
            if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
        }
    };

    // Store do Franchesco
    interacoes['F'] = [&](int px, int py, int larg) {
        NPCMerchant franchesco;
        franchesco.interagir(currentPlayer);
        Diary::instancia().registrarNPC("Franchesco (Merchant)");
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    };

    // Forja do Bjorn
    interacoes['B'] = [&](int px, int py, int larg) {
        NPCBlacksmith bjorn;
        bjorn.interagir(currentPlayer);
        Diary::instancia().registrarNPC("Bjorn (Blacksmith)");
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    };

    // Cavaleiro Generico (Treino)
    interacoes['C'] = [&](int px, int py, int larg) {
        std::vector<std::string> falas = {
            "Saudacoes, guerreiro!",
            "Deseja treinar suas habilidades em um combat amistoso?",
            "Esta luta nao concede experiencia (XP) ou recompensas permanentes,",
            "mas serve como um otimo teste de suas taticas."
        };
        int escolha = InputControl::lerSelecaoMenuEmPopup("TREINO DE COMBATE", falas, {"Aceitar Treino", "Recusar"}, Color::GRAY);
        if (escolha == 0) {
            std::vector<std::unique_ptr<Character>> enemies;
            enemies.push_back(NPCGenericKnight::criarCavaleiro("Cavaleiro de Treino"));

            int xpAntes = currentPlayer->getXpAtual();
            int ouroAntes = currentPlayer->obterInventario()->obterOuro();

            Combat combat(currentPlayer, std::move(enemies));
            if (MapControllera::isExploracao3DAtiva()) {
                combat.setContexto3D(
                    true, 
                    matrizDoMapaAtual, 
                    MapControllera::obterPosCamera3DX(), 
                    MapControllera::obterPosCamera3DY(), 
                    MapControllera::obterAnguloCamera3D(), 
                    MapControllera::obterTituloMapaAtual()
                );
            }
            combat.iniciarCombate();

            // Restaura o progress de XP e Ouro para garantir que nao ganhe nada permanente
            currentPlayer->definirXpAtual(xpAntes);
            int ouroDepois = currentPlayer->obterInventario()->obterOuro();
            currentPlayer->obterInventario()->adicionarOuro(ouroAntes - ouroDepois);
        }
        
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    };

    // Store de appearance (Anok)
    interacoes['N'] = [&](int px, int py, int larg) {
        NPCAparencia appearance;
        appearance.interagir(currentPlayer);
        Diary::instancia().registrarNPC("Anok (Estilista)");
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    };

    // Alchemist
    interacoes['Q'] = [&](int px, int py, int larg) {
        NPCAlchemist alquimista;
        alquimista.interagir(currentPlayer);
        Diary::instancia().registrarNPC("Alchemist Real");
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    };

    auto processarInteracao = [&](int px, int py, int larg) {
        char celulaDestino = matrizDoMapaAtual[py][px];
        auto it = interacoes.find(celulaDestino);
        if (it != interacoes.end()) {
            it->second(px, py, larg);
        } else {
            int oldX = posicaoXDoJogador;
            int oldY = posicaoYDoJogador;
            MapHelper::processarMovimento(posicaoXDoJogador, posicaoYDoJogador, px, py, celulaDestino, "*=|[] ");
            if ((posicaoXDoJogador != oldX || posicaoYDoJogador != oldY) && !jogadorEstaDentroDeUmSubMapa && py >= static_cast<int>(matrizDoMapaAtual.size()) - 3) {
                proximoMapa = ProximaTransicaoMapa::PonteReino;
                exploracaoEstaAtiva = false;
            }
        }
    };

    bool precisaRenderizar = true;
    ProximaTransicaoMapa destinoViagemRapida = MapControllera::executarLoopDeExploracao(
        currentPlayer, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual, []() { return ""; },
        [this]() -> std::vector<std::string> { 
            if (jogadorEstaDentroDeUmSubMapa) return Mapa4ReinoLayouts::obterLayoutIgreja();
            return Mapa4ReinoLayouts::obterLayoutReino();
        },
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
