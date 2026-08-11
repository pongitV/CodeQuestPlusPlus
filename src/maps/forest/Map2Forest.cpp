#include "Map2Forest.h"
#include "../utils/MapHelper.h"
#include "../../core/state/Debug.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <chrono>
#include <thread>

#include "../../ui/screens/menu/ScreenMenu.h"
#include "../../systems/inventory/Item.h"
#include "../../systems/inventory/equipment/ArmorEquipment.h"
#include "../../systems/inventory/items/ConsumableItem.h"
#include "../../systems/inventory/items/MaterialItem.h"
#include "../../core/state/EnemyCreator.h"
#include "../../systems/inventory/InventoryCombat.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/bestiary/ScreenBestiary.h"
#include "../../systems/combat/Combat.h"
#include "../control/MapController.h"
#include "../engine/MapAnimator.h"
#include "../engine/MapLoader.h"
#include "../../entities/npcs/mage-npc/NPCMageNPC.h"
#include "../../entities/enemies/forest-abomination/ForestAbomination.h"
#include "../../entities/enemies/mahoraga/Mahoraga.h"
#include "../../entities/enemies/ClassBaseEnemy.h"
#include "../../systems/progress/Diary.h"
#include "../../core/utils/DialogFunctions.h"
#include "../control/MapController.h"
#include "../../core/utils/InputControl.h"
#include "../../core/utils/RandomGenerator.h"
#include "../kingdom/Map3KingdomBridge.h"
#include "Map2ForestLayout.h"
#include "../../core/utils/Color.h"

Mapa2Floresta::Mapa2Floresta(Character* personagemJogador) :
    posicaoXDoJogador(31), 
    posicaoYDoJogador(17),
    currentPlayer(personagemJogador), 
    posicaoXSalvaAntesDeEntrarNoSubMapa(0), 
    posicaoYSalvaAntesDeEntrarNoSubMapa(0),
    jogadorEstaDentroDeUmSubMapa(false),
    coracaoDaArvoreJaFoiVisitado(false), 
    labirintoJaFoiVisitado(false),
    salaDoChefeJaFoiVisitada(false),
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("FLORESTA"),
    proximoMapa(ProximaTransicaoMapa::Nenhuma)
{
    matrizDoMapaAtual = Mapa2FlorestaLayouts::obterLayoutFloresta();
    MapLoadera::padronizarTamanhoDoMapa(matrizDoMapaAtual);
}

Mapa2Floresta::~Mapa2Floresta() = default;

namespace {
    class InteracaoSlime : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.proximaPosicaoX > 0 && ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX-1] != '^') {
                MapControllera::processarCombate(ctx.self->currentPlayer, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO PEGAJOSO", "Voce encontrou Slimes selvagens!", EnemyCreator::createSlimeEnemy(RandomGenerator::getInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoFada : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            MapControllera::processarCombate(ctx.self->currentPlayer, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO MAGICO", "Voce encontrou Fadas hostis!", EnemyCreator::createFairyEnemy(RandomGenerator::getInteiro(1, 3)), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoAbominacao : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            MapControllera::processarCombate(ctx.self->currentPlayer, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "ENCONTRO BOSS", "Voce encontrou a Abominacao da Forest!", EnemyCreator::createForestAbominationEnemy(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
        }
    };

    class InteracaoMorgana : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            NPCMageNPC interacaoMorgana;
            interacaoMorgana.interagir(ctx.self->currentPlayer);
            Diary::instancia().registrarNPC("Morgana (Bruxa)");
            if (ctx.self->exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
        }
    };

    class InteracaoBau : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            if (ctx.self->tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                
                std::vector<std::string> msgText = { "Voce encontrou um Bau ancestral!" };
                std::vector<std::string> opcoesBau = { "Nao", "Abrir!" };
                int opcao = InputControl::lerSelecaoMenuEmPopup("TESOURO ESCONDIDO", msgText, opcoesBau, Color::GREEN);

                if (opcao == 1) {
                    if (RandomGenerator::rolarChance(25)) {
                        MapControllera::processarCombate(ctx.self->currentPlayer, ctx.self->matrizDoMapaAtual, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->exploracaoEstaAtiva, "CILADA!", "O Bau era um Mimic!", EnemyCreator::createMimicEnemy(1), ctx.proximaPosicaoX, ctx.proximaPosicaoY, ctx.proximaPosicaoX, 1, ctx.larguraDoTerminal, ctx.restaurarTela);
                    } else {
                    std::vector<std::string> lootMsg = { "O bau se abre rangendo...", "Voce obteve itens valiosos!", "" };

                    int qtdPocoes = RandomGenerator::getInteiro(2, 4);
                    for (int i = 0; i < qtdPocoes; ++i) {
                        auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%VM)");
                        pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
                        ctx.self->currentPlayer->obterInventario()->adicionarItem(std::move(pocao));
                    }
                    lootMsg.push_back("+ " + std::to_string(qtdPocoes) + "x Pocoes de Cura (30%VM)");

                    int qtdOuro = RandomGenerator::getInteiro(150, 300);
                    ctx.self->currentPlayer->obterInventario()->adicionarOuro(qtdOuro);
                    lootMsg.push_back("+ " + std::to_string(qtdOuro) + "G");

                    bool isFuria = RandomGenerator::rolarChance(50);
                    std::string nomeBuff = isFuria ? "Pocao de Furia (Buff)" : "Elixir Arcano (Buff)";
                    auto buff = std::make_unique<ItemConsumivel>(nomeBuff);
                    buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
                    ctx.self->currentPlayer->obterInventario()->adicionarItem(std::move(buff));
                    lootMsg.push_back("+ 1x " + nomeBuff);

                    ctx.self->currentPlayer->obterInventario()->adicionarItem(std::make_unique<MaterialItem>("Pedra magica de upgrade"));
                    lootMsg.push_back("+ 1x Pedra magica de upgrade");

                    ctx.self->matrizDoMapaAtual[ctx.proximaPosicaoY][ctx.proximaPosicaoX] = ' ';
                    ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                    ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
                    }
                }
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;

                if (ctx.self->exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    class InteracaoTeleporte : public InteracaoFloresta {
    public:
        void processar(ContextoInteracaoFloresta& ctx) override {
            int px = ctx.proximaPosicaoX;
            int py = ctx.proximaPosicaoY;
            std::string titulo = ctx.self->tituloDoMapaAtual;
            
            // 2. Voltar para a Village a partir da Forest
            if (px < 40 && py < 20 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Village;
            }
            // 3. Entrar no Coracao da Arvore a partir da Forest
            else if (px > 80 && py > 20 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                MapLoadera::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoCoracaoDaArvoreSalva, ctx.self->coracaoDaArvoreJaFoiVisitado, Mapa2FlorestaLayouts::obterLayoutCoracaoDaArvore(), 10, 3, "CORACAO DA ARVORE", ctx.restaurarTela);
            }
            // 4. Ir para o Kingdom a partir da Forest
            else if (px < 40 && py > 20 && !ctx.self->jogadorEstaDentroDeUmSubMapa) {
                ctx.self->exploracaoEstaAtiva = false;
                ctx.self->proximoMapa = ProximaTransicaoMapa::Kingdom;
            }
            // 5. Entrar no Labirinto a partir da Forest
            else if (px > 100 && py < 20 && titulo == "FLORESTA") {
                if (!ctx.self->currentPlayer->obterLabirintoDesbloqueado()) {
                    std::vector<std::string> msg = {
                        "A passagem esta selada por magia.",
                        "Fale com Morgana."
                    };
                    ctx.self->posicaoXDoJogador = 129;
                    ctx.self->posicaoYDoJogador = 10;
                    if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
                    return;
                }

                MapLoadera::entrarSubMapa(ctx.self->matrizDoMapaAtual, ctx.self->matrizDoMapaPrincipalSalva, ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa, ctx.self->posicaoXDoJogador, ctx.self->posicaoYDoJogador, ctx.self->jogadorEstaDentroDeUmSubMapa, ctx.self->tituloDoMapaAtual, ctx.self->matrizDoMapaDoLabirintoSalva, ctx.self->labirintoJaFoiVisitado, Mapa2FlorestaLayouts::obterLayoutLabirinto(), 4, 11, "LABIRINTO SUBTERRANEO", ctx.restaurarTela);
            }
            // 6. Sair de Submapas
            else if ((titulo == "CORACAO DA ARVORE") ||
                     ((px == 1 || px == 2) && py == 11 && titulo == "LABIRINTO SUBTERRANEO") ||
                     (titulo == "SALA DO CHEFE")) {
                
                if (titulo == "CORACAO DA ARVORE") {
                    ctx.self->coracaoDaArvoreJaFoiVisitado = false; 
                }
                else if (titulo == "LABIRINTO SUBTERRANEO") {
                    ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                }
                else if (titulo == "SALA DO CHEFE") {
                    ctx.self->matrizDoMapaSalaDoChefeSalva = ctx.self->matrizDoMapaAtual;
                }

                if (titulo == "SALA DO CHEFE") {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaDoLabirintoSalva;
                    ctx.self->posicaoXDoJogador = 76; 
                    ctx.self->posicaoYDoJogador = 11;
                    ctx.self->tituloDoMapaAtual = "LABIRINTO SUBTERRANEO";
                } else {
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                    ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                    ctx.self->tituloDoMapaAtual = "FLORESTA";
                }
                if (!MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
            }
            // 7. Fim do Labirinto (Escadaria para Boss)
            else if ((px == 77 || px == 78) && py == 11 && titulo == "LABIRINTO SUBTERRANEO") {
                std::vector<std::string> msgLab = {
                    "Voce encontrou a saida do labirinto!",
                    "A sua frente, uma escadaria desce para uma caverna escura.",
                    "No fundo, parece haver um mar de liquido preto raso..."
                };
                std::vector<std::string> opcoesCaminho = { "Descer a escadaria", "Voltar para a Forest" };
                int escolha = InputControl::lerSelecaoMenuEmPopup("FIM DO LABIRINTO", msgLab, opcoesCaminho, Color::GREEN);

                if (escolha == 0) {
                    std::vector<std::string> msgBoss = {
                        "O ar aqui embaixo e gelado, cortante.",
                        "O liquido preto no chao e raso e liso como vidro.",
                        "Tudo e escuridao, exceto pelo brilho pulsante da",
                        "enorme runa magica desenhada no fundo da caverna."
                    };
                    
                    std::vector<std::string> opcoesBoss = {
                        std::string("Seguir em frente"),
                        std::string("Voltar para a seguranca da Forest")
                    };
                    int escolhaBoss = InputControl::lerSelecaoMenuEmPopup("CAVERNA SOMBRIA", msgBoss, opcoesBoss, Color::RED);
                    
                    if (escolhaBoss == 0) {
                        ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                        if (!ctx.self->salaDoChefeJaFoiVisitada) {
                            ctx.self->matrizDoMapaAtual = Mapa2FlorestaLayouts::obterLayoutSalaDoChefe();
                            MapLoadera::padronizarTamanhoDoMapa(ctx.self->matrizDoMapaAtual);
                            ctx.self->salaDoChefeJaFoiVisitada = true;
                        } else {
                            ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaSalaDoChefeSalva;
                        }
                        ctx.self->posicaoXDoJogador = 53;
                        ctx.self->posicaoYDoJogador = 53;
                        ctx.self->tituloDoMapaAtual = "SALA DO CHEFE";
                        if (!MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
                    } else {
                        ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                        ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                        ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                        ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                        ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                        ctx.self->tituloDoMapaAtual = "FLORESTA";
                    }
                } else {
                    ctx.self->matrizDoMapaDoLabirintoSalva = ctx.self->matrizDoMapaAtual;
                    ctx.self->matrizDoMapaAtual = ctx.self->matrizDoMapaPrincipalSalva;
                    ctx.self->posicaoXDoJogador = ctx.self->posicaoXSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->posicaoYDoJogador = ctx.self->posicaoYSalvaAntesDeEntrarNoSubMapa;
                    ctx.self->jogadorEstaDentroDeUmSubMapa = false;
                    ctx.self->tituloDoMapaAtual = "FLORESTA";
                }
                if (ctx.self->exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) if (!MapControllera::isExploracao3DAtiva()) ctx.restaurarTela();
            } else {
                ctx.self->posicaoXDoJogador = ctx.proximaPosicaoX;
                ctx.self->posicaoYDoJogador = ctx.proximaPosicaoY;
            }
        }
    };

    std::vector<std::string> obterLayoutOriginalFloresta(const std::string& titulo) {
        if (titulo == "CORACAO DA ARVORE") return Mapa2FlorestaLayouts::obterLayoutCoracaoDaArvore();
        if (titulo == "LABIRINTO SUBTERRANEO") return Mapa2FlorestaLayouts::obterLayoutLabirinto();
        if (titulo == "SALA DO CHEFE") return Mapa2FlorestaLayouts::obterLayoutSalaDoChefe();
        return Mapa2FlorestaLayouts::obterLayoutFloresta();
    }
}

void Mapa2Floresta::initializeInteracoes() {
    interacoes['S'] = std::make_unique<InteracaoSlime>();
    interacoes['F'] = std::make_unique<InteracaoFada>();
    interacoes['A'] = std::make_unique<InteracaoAbominacao>();
    interacoes['M'] = std::make_unique<InteracaoMorgana>();
    interacoes['B'] = std::make_unique<InteracaoBau>();
    interacoes['^'] = std::make_unique<InteracaoTeleporte>();
}

ProximaTransicaoMapa Mapa2Floresta::iniciarLoopDeExploracao()
{
    // Resgata o jogador se ele usou Viagem Rapida enquanto estava dentro de um submapa
    if (jogadorEstaDentroDeUmSubMapa) {
        matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
        posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
        posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
        jogadorEstaDentroDeUmSubMapa = false;
        tituloDoMapaAtual = "FLORESTA";
    }

    initializeInteracoes();

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
        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Color::GREEN, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false, true, nullptr);
        precisaRenderizar = true;
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        std::vector<std::string> arteTrans;
        int larguraTrans = 0;

        if (tituloDoMapaAtual == "FLORESTA") {
            arteTitulo = Mapa2FlorestaLayouts::obterLogoFloresta();
            larguraArte = 100;
            arteTrans = Mapa2FlorestaLayouts::obterArteTransicaoFloresta();
            larguraTrans = 87;
        }

        linhaInicialParaDesenharOMapa = MapAnimatora::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, arteTrans, larguraTrans, Color::GREEN, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, true, nullptr);
        precisaRenderizar = false;
    };

    animarTela();

    auto processarInteracao = [&](int proximaPosicaoX, int proximaPosicaoY, int larguraDoTerminal)
    {
        char celulaDestinoDoMapa = matrizDoMapaAtual[proximaPosicaoY][proximaPosicaoX];
        
        if (tituloDoMapaAtual == "SALA DO CHEFE" && (celulaDestinoDoMapa == 'M' || celulaDestinoDoMapa == 'A' || celulaDestinoDoMapa == 'H' || celulaDestinoDoMapa == 'O' || celulaDestinoDoMapa == 'R' || celulaDestinoDoMapa == 'G')) {
            std::vector<std::unique_ptr<Character>> bossMaho;
            auto bossMahoraga = std::make_unique<Character>("Mahoraga", std::make_unique<Mahoraga>(), std::make_unique<ClassBaseInimigo>());
            bossMahoraga->calcularAtributos();
            bossMahoraga->modificarVida(bossMahoraga->obterVidaMaxima());
            bossMaho.push_back(std::move(bossMahoraga));

            int startX = proximaPosicaoX;
            while (startX > 0 && (matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'M' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'A' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'H' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'O' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'R' || matrizDoMapaAtual[proximaPosicaoY][startX-1] == 'G')) startX--;

            MapControllera::processarCombate(currentPlayer, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, exploracaoEstaAtiva, "O GENERAL DIVINO", "A Roda comeca a girar... Mahoraga despertou!", std::move(bossMaho), proximaPosicaoX, proximaPosicaoY, startX, 8, larguraDoTerminal, restaurarTela);
            return;
        }
        
        auto it = interacoes.find(celulaDestinoDoMapa);
        if (it != interacoes.end()) {
            ContextoInteracaoFloresta ctx = {this, proximaPosicaoX, proximaPosicaoY, larguraDoTerminal, restaurarTela, celulaDestinoDoMapa, animarTela};
            it->second->processar(ctx);
        } else {
            if (tituloDoMapaAtual == "LABIRINTO SUBTERRANEO") {
                if (!MapHelper::ehParede(celulaDestinoDoMapa) || Debug::isNoclipActive) {
                    posicaoXDoJogador = proximaPosicaoX;
                    posicaoYDoJogador = proximaPosicaoY;
                }
            } else if (tituloDoMapaAtual == "SALA DO CHEFE") {
                if (celulaDestinoDoMapa != ' ' || Debug::isNoclipActive) {
                    posicaoXDoJogador = proximaPosicaoX;
                    posicaoYDoJogador = proximaPosicaoY;
                }
            } else {
                MapHelper::processarMovimento(posicaoXDoJogador, posicaoYDoJogador, proximaPosicaoX, proximaPosicaoY, celulaDestinoDoMapa);
            }
        }
    };

    ProximaTransicaoMapa destinoViagemRapida = MapControllera::executarLoopDeExploracao(
        currentPlayer, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual,
        [this]() { return (tituloDoMapaAtual == "SALA DO CHEFE") ? "" : "SFA"; },
        [this]() { return obterLayoutOriginalFloresta(tituloDoMapaAtual); },
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
