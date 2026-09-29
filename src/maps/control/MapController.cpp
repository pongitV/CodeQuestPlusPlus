#include "MapController.h"
#include "../utils/MapHelper.h"
#include "../../systems/inventory/InventoryCombat.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/diary/ScreenDiary.h"
#include "../../ui/screens/menu/ScreenMenu.h"
#include "../../ui/screens/pause/ScreenPause.h"
#include "../../core/utils/InputDispatcher.h"

#include "../../ui/screens/map-world/ScreenMapWorld.h"
#include "../../systems/combat/Combat.h"
#include "../../systems/combat/CombatRaycasterUIImpl.h"
#include "../../systems/progress/Progression.h"
#include "../../core/state/Debug.h"
#include "../../core/utils/InputControl.h"
#include "../../core/utils/RandomGenerator.h"
#include "../../core/utils/RendererProvider.h"
#include "../../entities/races/RaceBase.h"
#include "../engine/MapPhysics.h"
#include "../../rendering/raycaster/engine-raycaster/Raycaster.h"
#include "../../rendering/raycaster/engine-raycaster/RaycasterWorld.h"
#include "../../ui/UIManager.h"

#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>
#include <cmath>

#include "../engine/MapRenderer.h"
#include "../engine/MapInputController.h"
#include "../../core/utils/Color.h"

namespace {
    std::string extrairCorBaseDoRaycaster(char celula, const std::string& tituloDoMapa, bool isFloresta) {
        return "";
    }
}

static bool s_recemTrocouDeMapa = false;
static float s_posCamera3DX = -1.0f;
static float s_posCamera3DY = -1.0f;
static float s_anguloCamera3D = 0.0f;
static std::string s_tituloMapaAtual = "";
static std::vector<std::string> s_matrizDoMapaAtual;

void MapControllera::sinalizarTrocaDeMapa3D() { s_recemTrocouDeMapa = true; }
bool MapControllera::isExploracao3DAtiva() { return GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva(); }
float MapControllera::obterPosCamera3DX() { return s_posCamera3DX; }
float MapControllera::obterPosCamera3DY() { return s_posCamera3DY; }
float MapControllera::obterAnguloCamera3D() { return s_anguloCamera3D; }
std::string MapControllera::obterTituloMapaAtual() { return s_tituloMapaAtual; }
std::vector<std::string> MapControllera::obterMatrizDoMapaAtual() { return s_matrizDoMapaAtual; }

// processarInputEComandos movido para MapInputControllera.cpp

// aplicarLimitesDeMapa foi movido para MapPhysicsa
void MapControllera::processarCombate(
    Character* currentPlayer, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<Character>> inimigosParaBatalha, 
    int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int amountDeCelulasOcupadas, int /*larguraDoTerminal*/, const std::function<void()>& restaurarTela)
{
    std::vector<std::string> texto = { 
        std::string("[!] ") + mensagemDeAviso
    };
    std::vector<std::string> opcoesCombate = { "Nao, recuar", "Sim, batalha!" };
    
    int opcaoEscolhidaPeloJogador = 0;
    if (RendererProvider::get()) {
        opcaoEscolhidaPeloJogador = RendererProvider::get()->lerSelecaoMenuEmPopup(tituloDoCombate, texto, opcoesCombate, Color::RED);
    } else {
        opcaoEscolhidaPeloJogador = InputControl::lerSelecaoMenuEmPopup(tituloDoCombate, texto, opcoesCombate, Color::RED);
    }

    if (opcaoEscolhidaPeloJogador == 1) {
        std::unique_ptr<ICombateUI> ui = nullptr;
        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            ui = std::make_unique<CombateRaycasterUIImpl>();
        }
        
        Combat combat(currentPlayer, std::move(inimigosParaBatalha), std::move(ui));
        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            combat.setContexto3D(true, matrizDoMapaAtual, s_posCamera3DX, s_posCamera3DY, s_anguloCamera3D, s_tituloMapaAtual);
        }
        combat.iniciarCombate();

        if (currentPlayer->obterVida() > 0) {
            for (int i = 0; i < amountDeCelulasOcupadas; ++i) matrizDoMapaAtual[posicaoYAposCombate][posicaoXInicialDoInimigo + i] = '.';
            posicaoXDoJogador = posicaoXAposCombate;
            posicaoYDoJogador = posicaoYAposCombate;
        }
    }

    if (exploracaoEstaAtiva && !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) restaurarTela();
}

// animarIntroducaoMapa movido para MapAnimatora.cpp}


// animarFlashbang movido para MapAnimatora.cpp

// Funcoes da camera e renderizacao abstraidas para RenderizadorMapa.cpp

std::string MapControllera::formatarCelula(char celula, int x, int y, const std::string& tituloDoMapa, const std::vector<std::string>& matrizDoMapa, bool isMinimapa) {
    thread_local std::string ultimoTitulo = "";
    thread_local std::string tituloUpper = "";
    thread_local bool isReino = false, isInterior = false, isFloresta = false, isVila = false, isSpawn = false;
    thread_local bool isChefe = false, isCoracao = false, isLabirinto = false, isCaverna = false, isSalaChefe = false;

    if (ultimoTitulo != tituloDoMapa) {
        ultimoTitulo = tituloDoMapa;
        tituloUpper = tituloDoMapa;
        for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));
        
        isReino = (tituloUpper.find("KINGDOM") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);
        isFloresta = (tituloUpper.find("FLORESTA") != std::string::npos);
        isVila = (tituloUpper.find("VILA") != std::string::npos);
        isSpawn = (tituloUpper.find("INICIO") != std::string::npos);
        isChefe = (tituloUpper.find("CHEFE") != std::string::npos);
        isCoracao = (tituloUpper.find("CORACAO") != std::string::npos);
        isLabirinto = (tituloUpper.find("LABIRINTO") != std::string::npos);
        isCaverna = (tituloUpper.find("CAVERNA") != std::string::npos);
        isSalaChefe = (tituloUpper == "SALA DO CHEFE");
        isInterior = (isLabirinto || isChefe || isCoracao || isCaverna);
    }
    
    // Estetica engine IDE (visao 2D nativa)
    bool isEngineIDE = !isMinimapa && !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    if (isEngineIDE) {
        std::string npcs = "GOBFPMSTRCH";
        if (npcs.find(celula) == std::string::npos && celula != ' ' && !RaycasterWorld::isMapLabel(x, y, matrizDoMapa)) {
            
            if (celula == '.' && (!isInterior || isChefe || isCoracao)) {
                return "·"; // Traco do chao para a IDE
            }

            const char syntaxChars[] = "{};/*<>&|!=";
            int idx = (x * 7 + y * 13) % (sizeof(syntaxChars) - 1);
            char ideChar = syntaxChars[idx];
            
            if (isFloresta || celula == '*' || celula == '#') {
                return std::string(1, ideChar);
            }
            if (celula == '~') {
                return std::string(1, '~');
            }
            if (celula == '^') {
                return std::string(1, '^');
            }
            
            return std::string(1, ideChar);
        }
    }

    // Teleporte
    if (celula == '^') return std::string("^");
    
    // Agua
    if (celula == '~') return std::string("≈");
    
    // Arvores
    if (celula == '*') {
        bool isTrunk = false;
        if (y > 0 && matrizDoMapa[y-1][x] == '*') {
            int countHorizontal = 0;
            if (x > 0 && matrizDoMapa[y][x-1] == '*') countHorizontal++;
            if (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && matrizDoMapa[y][x+1] == '*') countHorizontal++;
            if (countHorizontal <= 1) isTrunk = true;
        }
        if (isTrunk) return std::string("█");
        return std::string("▲");
    }
    
    // Verifica se e uma letra de placa de chao (Label) ANTES de checar as entities
    if (RaycasterWorld::isMapLabel(x, y, matrizDoMapa)) {
        return std::string(1, celula);
    }
    
    // Entities
    if (isVila || isSpawn) {
        if (celula == 'G' || celula == 'O') return std::string(1, celula);
        if (celula == 'B') return std::string("B");
        if (celula == 'F' && x > 0 && matrizDoMapa[y][x-1] == '{') return std::string("F");
        if (celula == 'P') return std::string("P");
    } else if (isFloresta) {
        if (celula == 'S' && (!isInterior || isChefe)) return std::string("S");
        if (celula == 'F' || celula == 'A') return std::string(1, celula);
        if (celula == 'M') return std::string("M");
        if (celula == 'B') return std::string("B");
    } else if (isReino) {
        if (celula == 'T') return std::string("T");
        if (celula == 'G') return std::string("G");
        if (celula == 'C') return std::string("C");
    }
    
    if (isSalaChefe && (celula == 'M' || celula == 'A' || celula == 'H' || celula == 'O' || celula == 'R' || celula == 'G')) {
        return std::string(1, celula);
    }
    
    // Casas e Estruturas
    if (!isInterior && !isReino) {
        if (celula == '_') return std::string("▄");
        if (celula == '|' || celula == '[' || celula == ']') return std::string("█");
        std::string estruturas = "{}/\\<>;=-:+";
        if (estruturas.find(celula) != std::string::npos) return std::string(1, celula);
        
        if (celula == '#') {
            return std::string("█");
        }
    }
    
    // Kingdom
    if (isReino) {
        if (celula == '|') return std::string("█");
        std::string estruturas = "_[]{}/\\<>;=-+#";
        if (estruturas.find(celula) != std::string::npos) {
            return std::string("█");
        }
    }
    
    // Labirinto
    if (isInterior) {
        if (isLabirinto) {
            auto isHWall = [](char c) { return c == '=' || c == '.' || c == '\''; };
            auto isVWall = [](char c) { return c == '|' || c == '+' || c == 'S' || c == 'E'; };

            if (celula == '=') return std::string("─");
            if (celula == '|') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                if (right && left) return std::string("┼");
                if (right) return std::string("├");
                if (left) return std::string("┤");
                return std::string("│");
            }
            if (celula == '.') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapa.size()) && isVWall(matrizDoMapa[y+1][x]));
                
                if (left && right && down) return std::string("┬");
                if (right && down) return std::string("┌");
                if (left && down) return std::string("┐");
                if (left && right) return std::string("─");
                return std::string("█");
            }
            if (celula == '\'') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool up = (y > 0 && isVWall(matrizDoMapa[y-1][x]));
                
                if (left && right && up) return std::string("┴");
                if (right && up) return std::string("└");
                if (left && up) return std::string("┘");
                if (left && right) return std::string("─");
                return std::string("█");
            }
            if (celula == '+') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapa.size()) && isVWall(matrizDoMapa[y+1][x]));
                bool up = (y > 0 && isVWall(matrizDoMapa[y-1][x]));
                
                if (left && right && down && up) return std::string("┼");
                if (left && right && down) return std::string("┬");
                if (left && right && up) return std::string("┴");
                if (up && down && left) return std::string("┤");
                if (up && down && right) return std::string("├");
                return std::string("┼");
            }
        }
        else if (isCaverna) {
            if (celula == '#') return std::string("█");
            if (celula == '.') {
                if (isMinimapa) return ".";
                return "·";
            }
        }
    }
    
    // Chao / Labels
    if (celula == '.' && (!isInterior || isChefe || isCoracao)) {
        if (isMinimapa) return ".";
        return "·";
    }
    
    if (std::isalpha(celula) && celula != ' ' && celula != 'S' && celula != 'F' && celula != 'A' && celula != 'M' && celula != 'B' && celula != 'T' && celula != 'G' && celula != 'C') {
        return std::string(1, celula);
    }
    
    return std::string(1, celula);
}

// renderizarMapa abstraido para RenderizadorMapa.cpp

ProximaTransicaoMapa MapControllera::executarLoopDeExploracao(
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
) {
    auto ultimoMovimentoInimigos = std::chrono::steady_clock::now();
    ProximaTransicaoMapa destinoViagemRapida = ProximaTransicaoMapa::Nenhuma;
    s_anguloCamera3D = 0.0f;
    s_posCamera3DX = -1.0f;
    s_posCamera3DY = -1.0f;
    s_tituloMapaAtual = tituloDoMapaAtual;
    s_matrizDoMapaAtual = matrizDoMapaAtual;

    while (exploracaoEstaAtiva && currentPlayer->obterVida() > 0)
    {
        s_tituloMapaAtual = tituloDoMapaAtual;
        
        auto agora = std::chrono::steady_clock::now();
        bool tempoDeMoverInimigos = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoMovimentoInimigos).count() >= 800;

        if (tempoDeMoverInimigos) {
            MapPhysicsa::moverInimigosAleatoriamente(matrizDoMapaAtual, obterLayoutOriginal(), obterSimbolosInimigos(), posicaoXDoJogador, posicaoYDoJogador);
            ultimoMovimentoInimigos = std::chrono::steady_clock::now();
            precisaRenderizar = true;
        }

        int larguraDoTerminal = 120;
        
        if (precisaRenderizar && !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            int alturaDoTerminal = 40;

            RenderizadorMapa::renderizarMapa(matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, larguraDoTerminal, alturaDoTerminal, linhaInicialParaDesenharOMapa, formatador);

            precisaRenderizar = false;
        }

        char teclaPressionadaPeloJogador = '\0';
        bool processarInput = false;

            if (!GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva() && InputControl::teclaPressionada()) {
            teclaPressionadaPeloJogador = InputControl::lerTecla();
            processarInput = true;
        }

        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva() || (processarInput && (teclaPressionadaPeloJogador == 'v' || teclaPressionadaPeloJogador == 'V'))) {
            static std::string tituloAnterior = "";
            int tipoAnimacao = 0;
            
            bool trocandoDePerspectiva = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
            if (trocandoDePerspectiva) {
                tipoAnimacao = 1;
            }
            if (s_recemTrocouDeMapa || tituloAnterior != tituloDoMapaAtual) {
                tipoAnimacao = 1;
                tituloAnterior = tituloDoMapaAtual;
            }

            s_recemTrocouDeMapa = false;
            if (!GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                GerenciadorPerspectiva::obterInstancia().alternarVisao();
            }

                if (s_posCamera3DX == -1.0f || static_cast<int>(s_posCamera3DX) != posicaoXDoJogador || static_cast<int>(s_posCamera3DY) != posicaoYDoJogador) {
                    s_posCamera3DX = static_cast<float>(posicaoXDoJogador) + 0.5f;
                    s_posCamera3DY = static_cast<float>(posicaoYDoJogador) + 0.5f;
                }
                
                int hitX = -1, hitY = -1;
                char acaoPendente = Raycaster::iniciarExploracao3D(matrizDoMapaAtual, s_posCamera3DX, s_posCamera3DY, s_anguloCamera3D, tituloDoMapaAtual, currentPlayer, hitX, hitY, tipoAnimacao);
                
                posicaoXDoJogador = static_cast<int>(s_posCamera3DX);
                posicaoYDoJogador = static_cast<int>(s_posCamera3DY);
                
                bool isTrigger = false;
                if (hitX != -1 && hitY != -1) {
                    MapPhysicsa::aplicarLimitesDeMapa(hitX, hitY, matrizDoMapaAtual);
                    
                    char cell = matrizDoMapaAtual[hitY][hitX];
                    // Verifica se o jogador parou em um trigger (Enemies ou Teleportes ou Terminal)
                    std::string triggers = "^GOBFSAMTHRPCIQ";
                    if (triggers.find(cell) != std::string::npos) {
                        isTrigger = true;
                    }
                    
                    int posXantes = posicaoXDoJogador;
                    int posYantes = posicaoYDoJogador;
                    
                    processarInteracao(hitX, hitY, larguraDoTerminal); // Aciona o combate ou NPC caso o jogador tenha parado sobre um
                    
                    // So empurra o jogador para tras se a posicao nao mudou (evita sobrescrever teleportes)
                    if (isTrigger && GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva() && posicaoXDoJogador == posXantes && posicaoYDoJogador == posYantes) {
                        // Empurra o jogador para tras em 1 casa para evitar que ele fique preso no NPC
                        // E previne loops onde ele volta pro game ja interagindo
                        s_posCamera3DX = static_cast<float>(hitX) + 0.5f - cos(s_anguloCamera3D) * 1.5f;
                        s_posCamera3DY = static_cast<float>(hitY) + 0.5f - sin(s_anguloCamera3D) * 1.5f;
                        posicaoXDoJogador = static_cast<int>(s_posCamera3DX);
                        posicaoYDoJogador = static_cast<int>(s_posCamera3DY);
                    }
                }
                
                if (acaoPendente == 'M' || acaoPendente == 'I' || acaoPendente == 'F' || acaoPendente == 'B' || acaoPendente == 'C' || acaoPendente == 'J' || acaoPendente == 27) {
                    teclaPressionadaPeloJogador = acaoPendente;
                    processarInput = true;
                } else if (!isTrigger) {
                    restaurarTela();
                    precisaRenderizar = true;
                    continue;
                } else {
                    continue;
                }
        }

        if (processarInput) {
            if (teclaPressionadaPeloJogador == 'v' || teclaPressionadaPeloJogador == 'V') {
                continue;
            }

            if (teclaPressionadaPeloJogador == 'm' || teclaPressionadaPeloJogador == 'M') {
                LocalizacaoMapa loc = LocalizacaoMapa::VilaInicial;
                std::string tituloUpper = tituloDoMapaAtual;
                std::transform(tituloUpper.begin(), tituloUpper.end(), tituloUpper.begin(), ::toupper);
                
                if (tituloUpper.find("FLORESTA") != std::string::npos || 
                    tituloUpper.find("BOSQUE") != std::string::npos ||
                    tituloUpper.find("LABIRINTO") != std::string::npos ||
                    tituloUpper.find("CHEFE") != std::string::npos ||
                    tituloUpper.find("ARVORE") != std::string::npos) {
                    loc = LocalizacaoMapa::Forest;
                } else if (tituloUpper.find("Kingdom") != std::string::npos) {
                    loc = LocalizacaoMapa::Kingdom;
                } else if (tituloUpper.find("REINO") != std::string::npos) {
                    loc = LocalizacaoMapa::Kingdom;
                }
                
                int progressoVila = Progression::instance().obterProgressoVila(currentPlayer);
                int progressoFloresta = Progression::instance().obterProgressoFloresta(currentPlayer);
                int progressoPonteReino = Progression::instance().obterProgressoPonteReino(currentPlayer);
                int progressoReino = Progression::instance().obterProgressoReino(currentPlayer);

                ProximaTransicaoMapa destino = TelaMapaMundo::display(currentPlayer, loc, progressoVila, progressoFloresta, progressoPonteReino, progressoReino);

                if (destino != ProximaTransicaoMapa::Nenhuma) {
                    destinoViagemRapida = destino;
                    exploracaoEstaAtiva = false; // Sinaliza para sair do loop e processar a viagem
                    break;
                }
                // Se nenhum destino foi escolhido, apenas restaura a tela e continua a exploracao.
                if (!GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                    restaurarTela();
                    precisaRenderizar = true;
                }
                continue;
            }

            int proximaPosicaoX = posicaoXDoJogador;
            int proximaPosicaoY = posicaoYDoJogador;

            if (MapInputControllera::processarInputEComandos(teclaPressionadaPeloJogador, currentPlayer, proximaPosicaoX, proximaPosicaoY, restaurarTela)) continue;
            
            if (currentPlayer->obterVoltarProMenu()) break;

            MapPhysicsa::aplicarLimitesDeMapa(proximaPosicaoX, proximaPosicaoY, matrizDoMapaAtual);
            processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
            
            precisaRenderizar = true;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    return destinoViagemRapida;
}
