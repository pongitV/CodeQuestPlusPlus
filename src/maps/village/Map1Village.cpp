#include "Map1Village.h"
#include "../utils/MapHelper.h"
#include "../../core/state/Debug.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <chrono>
#include <thread>

#include "../forest/Map2Forest.h"
#include "../../ui/screens/menu/ScreenMenu.h"
#include "../../core/state/EnemyCreator.h"
#include "../../systems/combat/Combat.h"
#include "../../systems/inventory/Item.h"
#include "../../systems/inventory/InventoryCombat.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/bestiary/ScreenBestiary.h"
#include "../../entities/npcs/blacksmith/NPCBlacksmith.h"
#include "../../entities/npcs/merchant/NPCMerchant.h"
#include "../../entities/enemies/exiled-orc/ExiledOrc.h"
#include "../../core/utils/DialogFunctions.h"
#include "../engine/MapAnimator.h"
#include "../control/MapController.h"
#include "../engine/MapLoader.h"
#include "../../core/utils/RandomGenerator.h"
#include "../interfaces/MapInteraction.h"
#include "../../systems/progress/Progression.h"
#include "../../systems/progress/ProgressionFlags.h"
#include "../../systems/progress/Diary.h"
#include "Map1VillageLayout.h"
#include "../../core/utils/Color.h"

Mapa1Vila::Mapa1Vila(Character* personagemJogador) :
    posicaoXDoJogador(4), 
    posicaoYDoJogador(5), 
    currentPlayer(personagemJogador), 
    exploracaoEstaAtiva(true),
    tituloDoMapaAtual("CAMINHO DO INICIO"),
    posicaoXSalvaAntesDeEntrarNoSubMapa(10), 
    posicaoYSalvaAntesDeEntrarNoSubMapa(4),
    jogadorEstaDentroDeUmSubMapa(true),
    bjornResgatado(Progression::instance().getFlag(Flags::Village_BjornRescued)), 
    cavernaJaFoiVisitada(false),
    spawnJaFoiVisitado(true),
    proximoMapa(ProximaTransicaoMapa::Nenhuma),
    veioDaFloresta(false)
{
    matrizDoMapaPrincipalSalva = Mapa1VilaLayouts::obterLayoutVilaInicial();
    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaPrincipalSalva);

    matrizDoMapaAtual = Mapa1VilaLayouts::obterLayoutSpawn();
    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaAtual);

    mapaBaseDaVila = Mapa1VilaLayouts::obterLayoutVilaInicial();
    MapLoadera::padronizarTamanhoDoMapa(mapaBaseDaVila);

    if (!bjornResgatado) {
        for (auto& linha : matrizDoMapaPrincipalSalva) {
            std::replace(linha.begin(), linha.end(), 'B', 'P');
        }
        for (auto& linha : mapaBaseDaVila) {
            std::replace(linha.begin(), linha.end(), 'B', 'P');
        }
    }
}

Mapa1Vila::~Mapa1Vila() = default;

namespace {
    class InteracaoCombateGoblin : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            MapControllera::processarCombate(ctx.self->currentPlayer, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO INESPERADO", "Voce encontrou uma horda de Goblins!", EnemyCreator::createGoblinEnemy(RandomGenerator::getInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoCombateOrk : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            MapControllera::processarCombate(ctx.self->currentPlayer, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO NA CAVERNA", "Voce encontrou um Ork!", EnemyCreator::createExiledOrcEnemy(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class NPCInteractionBlacksmith : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                std::vector<std::string> falasBjorn = {
                    std::string("Bjorn:") + " Pelos deuses, muito obrigado por me salvar!",
                    std::string("Bjorn:") + " Passe na Forja e eu ajudarei voce!"
                };
                
                ctx.self->bjornResgatado = true;
                Progression::instance().setFlag(Flags::Village_BjornRescued, true);

                ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX] = '.';
                
                // Atualiza os maps salvos para que a Placa volte a ser o Bjorn na Village
                for (auto& linha : ctx.self->matrizDoMapaPrincipalSalva) {
                    std::replace(linha.begin(), linha.end(), 'P', 'B');
                }
                for (auto& linha : ctx.self->mapaBaseDaVila) {
                    std::replace(linha.begin(), linha.end(), 'P', 'B');
                }
            } else if (ctx.self->tituloDoMapaAtual == "VILA INICIAL") {
                NPCBlacksmith interacaoBjorn;
                interacaoBjorn.interagir(ctx.self->currentPlayer);
                Diary::instancia().registrarNPC("Bjorn (Blacksmith)");
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
            if (ctx.self->exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class NPCInteractionMerchant : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            NPCMerchant interacaoFranchesco;
            interacaoFranchesco.interagir(ctx.self->currentPlayer);
            Diary::instancia().registrarNPC("Franchesco (Merchant)");
            if (ctx.self->exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class InteracaoPlaca : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            std::vector<std::string> msg = {
                "A Forja esta fechada.",
                "Uma placa diz: 'Fui a caverna a leste'."
            };
            if (ctx.self->exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class InteracaoTeleporte : public InteracaoVila {
    public:
        void processar(ContextoInteracaoVila& ctx) override {
            
            char nextCell = ' ';
            if (ctx.proximaPosicaoX + 1 < static_cast<int>(ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY].length())) {
                nextCell = ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX+1];
            }
            int px = ctx.proximaPosicaoX;
            int py = ctx.proximaPosicaoY;
            
            // 1. Entrar no Caminho do Inicio (Spawn) a partir da Village Inicial (X=18, Y=5 ou Y=4)
            if (px >= 17 && px <= 19 && py <= 6 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                MapLoadera::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoSpawnSalva, ctx.self->spawnJaFoiVisitado, Mapa1VilaLayouts::obterLayoutSpawn(), 53, 7, "CAMINHO DO INICIO", ctx.animarTela);
            }
            // 2. Entrar na Caverna a partir da Village (Caverna fica a leste)
            else if (px > 50 && py < 30 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                MapLoadera::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDaCavernaSalva, ctx.self->cavernaJaFoiVisitada, Mapa1VilaLayouts::obterLayoutCaverna(ctx.self->bjornResgatado), 14, 3, "CAVERNA DO ORK", ctx.animarTela);
            }
            // 3. Retornar dos Interiores/Caverna de volta para a Village Inicial
            else if (nextCell == 'S' && ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (ctx.self->tituloDoMapaAtual == "CAVERNA DO ORK") {
                    ctx.self->cavernaJaFoiVisitada = false;
                }

                ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                MapLoadera::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
                ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                ctx.self->tituloDoMapaAtual = "VILA INICIAL";
                if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
                else MapControllera::sinalizarTrocaDeMapa3D();
            }
            // 3. Voltar para a Village Inicial a partir do Caminho do Inicio (X=54, Y=7 ou Y=6)
            else if (px == 54 && (py == 7 || py == 6) && ctx.self->tituloDoMapaAtual == "CAMINHO DO INICIO") {
                ctx.self->matrizDoMapaDoSpawnSalva = ctx.self->matrizDoMapaAtual;
                ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                MapLoadera::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
                ctx.self->posicaoXDoJogador = 17;
                ctx.self->posicaoYDoJogador = 5;
                ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                ctx.self->tituloDoMapaAtual = "VILA INICIAL";
                if (!MapControllera::isExploracao3DAtiva()) ctx.animarTela();
                else MapControllera::sinalizarTrocaDeMapa3D();
            }
            // 5. Ir para a Forest a partir da Village
            else if (py >= 30 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                if (!Progression::instance().getFlag(Flags::Village_BjornRescued)) {
                    std::vector<std::string> msg = {
                        "Voce precisa ajudar os habitantes da vila antes de seguir jornada.",
                        "(Dica: Explore a caverna a leste da vila)."
                    };
                    return;
                }
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Forest;
                ctx.self->veioDaFloresta = true;
            }
            else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    std::vector<std::string> obterLayoutOriginalVila(const std::string& titulo, bool bjornResgatado) {
        if (titulo == "CAVERNA DO ORK") return Mapa1VilaLayouts::obterLayoutCaverna(bjornResgatado);
        if (titulo == "CAMINHO DO INICIO") return Mapa1VilaLayouts::obterLayoutSpawn();
        auto layout = Mapa1VilaLayouts::obterLayoutVilaInicial();
        if (!bjornResgatado) {
            for (auto& linha : layout) {
                std::replace(linha.begin(), linha.end(), 'B', 'P');
            }
        }
        return layout;
    }
}

void Mapa1Vila::initializeInteracoes() {
    interacoes['G'] = std::make_unique<InteracaoCombateGoblin>();
    interacoes['O'] = std::make_unique<InteracaoCombateOrk>();
    interacoes['B'] = std::make_unique<NPCInteractionBlacksmith>();
    interacoes['F'] = std::make_unique<NPCInteractionMerchant>();
    interacoes['P'] = std::make_unique<InteracaoPlaca>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
    interacoes['S'] = std::make_unique<InteracaoTeleporte>();
    interacoes['C'] = std::make_unique<InteracaoTeleporte>();
}

ProximaTransicaoMapa Mapa1Vila::iniciarLoopDeExploracao()
{
    initializeInteracoes();

    if (veioDaFloresta) {
        matrizDoMapaAtual = mapaBaseDaVila;
        cavernaJaFoiVisitada = false;
        veioDaFloresta = false;
    }

    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaAtual);


    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
            char ic = '@';
            if (ic <= 32 || ic > 126) ic = '@'; 
            return "" + std::string(1, ic) + "";
        }
        return MapControllera::formatarCelula(celula, x, y, tituloDoMapaAtual, matrizDoMapaAtual, false);
    };

    bool precisaRenderizar = false;
    int linhaInicialParaDesenharOMapa = 0;

    auto restaurarTela = [&]() {
        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Color::YELLOW, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false, true, nullptr);
        precisaRenderizar = true;
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        std::vector<std::string> arteTrans;
        int larguraTrans = 0;
        std::function<void()> acaoNarracao = nullptr;
        bool usarAnimacaoBanner = true;

        if (tituloDoMapaAtual == "VILA INICIAL") {
            arteTitulo = Mapa1VilaLayouts::obterLogoVila();
            larguraArte = 125;
            arteTrans = Mapa1VilaLayouts::obterArteTransicaoVila();
            larguraTrans = 75;
        } else if (tituloDoMapaAtual == "CAMINHO DO INICIO") {
            arteTitulo = Mapa1VilaLayouts::obterLogoSpawn();
            larguraArte = 105;
            if (matrizDoMapaDoSpawnSalva.empty()) {
                usarAnimacaoBanner = false;
            }
        }
        
        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Color::YELLOW, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, usarAnimacaoBanner, acaoNarracao);
        precisaRenderizar = false;
    };

    // Substitui o Bjorn por uma placa se ele ainda nao foi resgatado (Antes de animar a screen)
    if (tituloDoMapaAtual == "VILA INICIAL" && !bjornResgatado) {
        for (auto& linha : matrizDoMapaAtual) {
            std::replace(linha.begin(), linha.end(), 'B', 'P');
        }
    }

    animarTela();

    if (mapaBaseDaVila.empty()) mapaBaseDaVila = matrizDoMapaPrincipalSalva;

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal) {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        auto it = interacoes.find(celulaDestinoDoMapa);
        bool ehFalsoF = (celulaDestinoDoMapa == 'F' && proximaPosicaoX > 0 && matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX - 1] != '{');

        if (it != interacoes.end() && !ehFalsoF) {
            ContextoInteracaoVila ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa, animarTela};
            it->second->processar(ctx);
        } else {
            MapHelper::processarMovimento(posicaoXDoJogador, posicaoYDoJogador, proximaPosicaoX, proximaPosicaoY, celulaDestinoDoMapa);
        }
    };

    ProximaTransicaoMapa destinoViagemRapida = MapControllera::executarLoopDeExploracao(
        currentPlayer, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual,
        [this]() { return "GO"; },
        [this]() { return obterLayoutOriginalVila(tituloDoMapaAtual, bjornResgatado); },
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
