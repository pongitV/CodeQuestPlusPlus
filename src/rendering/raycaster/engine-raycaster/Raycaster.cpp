#include "Raycaster.h"
#include "../../../core/window/GameWindow.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/input/InputSystem.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../systems/inventory/InventoryCombat.h"
#include "../../../ui/screens/attributes/ScreenAttributes.h"
#include "../../../ui/screens/diary/ScreenDiary.h"
#include "../../../ui/screens/pause/ScreenPause.h"
#include "RaycasterSprites.h"
#include "RaycasterEnemies.h"
#include "RaycasterNPCs.h"
#include "RaycasterWorld.h"
#include "RaycasterHUD.h"
#include "RaycasterRenderer.h"
#include "../../../core/state/Debug.h"
#include "RaycasterControls.h"
#include <map>
#include <iostream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <thread>
#include <string_view>
#include <cstring>
#include "../../../core/d2d-context/D2DContext.h"
#include "../../direct-2d/GridEmulator2D.h"
#include "../../../core/window/GameWindow.h"
#include "../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../rendering/direct-2d/RaycasterD2DUtils.h"
#include "../../../rendering/direct-2d/UIRenderer2D.h"



#include "../../../maps/village/Map1VillageLayout.h"
#include "../../../maps/forest/Map2ForestLayout.h"
#include "../../../maps/kingdom/Map3KingdomBridgeLayout.h"
#include "../../../maps/kingdom/Map4KingdomLayout.h"
#include "../../../core/utils/Color.h"

using namespace std;

std::atomic<float> Raycaster::sensibilidadeX{0.0009f}; // 45%
std::atomic<float> Raycaster::sensibilidadeY{0.0012f}; // 15%

const std::vector<std::string>* Raycaster::s_mapaAtual = nullptr;
float Raycaster::s_jogadorXAtual = 0.0f;
float Raycaster::s_jogadorYAtual = 0.0f;
float Raycaster::s_anguloAtual = 0.0f;
std::string Raycaster::s_tituloAtual = "";
Character* Raycaster::s_currentPlayer = nullptr;
float Raycaster::s_tempoAbsolutoInicial = 0.0f;
std::chrono::steady_clock::time_point Raycaster::s_tpInicio;



std::string RaycasterFrame::s_ultimoQuadroRenderizado;

void RaycasterFrame::restaurarUltimoQuadro() {
    if (s_ultimoQuadroRenderizado.empty()) return;
}

static std::vector<std::string> obterArteTituloMapaRaycaster(const std::string& tituloMapa) {
    if (tituloMapa == "VILA INICIAL") return Mapa1VilaLayouts::obterLogoVila();
    if (tituloMapa == "CAMINHO DO INICIO") return Mapa1VilaLayouts::obterLogoSpawn();
    if (tituloMapa == "FLORESTA" || tituloMapa == "LABIRINTO SUBTERRANEO" || tituloMapa == "SALA DO CHEFE") return Mapa2FlorestaLayouts::obterLogoFloresta();
    if (tituloMapa == "PONTE DO REINO" || tituloMapa == "CAMINHO DO Kingdom") return Mapa3PonteReinoLayouts::obterLogoPonteReino();
    if (tituloMapa == "REINO" || tituloMapa.find("Kingdom") != std::string::npos || tituloMapa.find("REINO") != std::string::npos) return Mapa4ReinoLayouts::obterLogoReino();
    return {};
}

static inline void writeAnsiPixel(std::string& s, const Pixel3D& px) {
    s = std::string(1, px.ch);
}

static void downsampleTelaBuffer(const vector<Pixel3D>& tela3D, vector<string>& screen, int LARGURA_TELA, int ALTURA_TELA) {
    for (int y = 0; y < ALTURA_TELA; y++) {
        for (int x = 0; x < LARGURA_TELA; x++) {
            const Pixel3D& top = tela3D[(y * 2) * LARGURA_TELA + x];
            screen[y * LARGURA_TELA + x] = std::string(1, top.ch);
        }
    }
}

char Raycaster::iniciarExploracao3D(const vector<string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const string& tituloMapa, Character* jogador, int& outHitX, int& outHitY, int tipoAnimacaoEntrada) {
    outHitX = -1;
    outHitY = -1;
    if (matrizDoMapa.empty() || !jogador) return 0;

    s_mapaAtual = &matrizDoMapa;
    s_jogadorXAtual = jogadorX;
    s_jogadorYAtual = jogadorY;
    s_anguloAtual = anguloVisao;
    s_tituloAtual = tituloMapa;
    s_currentPlayer = jogador;

    bool temaFloresta = RaycasterWorld::isTemaFloresta(tituloMapa);
    int temaCeu = RaycasterWorld::obterTemaCeu(tituloMapa);

    int LARGURA_TELA = 640;
    int ALTURA_TELA = 360;
    
    const auto& cacheSprites = RaycasterNPCs::obterCacheGlobal();



    float profundidadeMaxima = 150.0f;
    float velocidadeMovimento = 5.0f;
    float animacaoTituloMapaTimer = 0.0f;
    float animacaoTituloMapaDuracao = 4.0f; 
    std::vector<std::string> arteTituloMapa;

    int ALTURA_INTERNA = ALTURA_TELA;
    vector<Pixel3D> tela3D(LARGURA_TELA * ALTURA_INTERNA);

    bool hasFocus = true;
    if (auto* win = D2DContext::window) {
        HWND h = win->obterHWND();
        RECT rc;
        GetWindowRect(h, &rc);
        ClipCursor(&rc);
    }
    GameWindow::ocultarCursor();

    auto tp1 = chrono::steady_clock::now();
    auto tp2 = chrono::steady_clock::now();
    static auto tempoInicio = chrono::steady_clock::now();
    s_tpInicio = tempoInicio;
    float bobbingTime = 0.0f;
    float bobbingAmplitude = 0.0f;
    float pitchOffset = 0.0f;
    int bobbingOffset = 0;

    int temaAtivoInicial = temaCeu;
    if (temaCeu == 1 || temaCeu == 2) {
        long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float anguloGlobal = ((globalMs % 60000) / 60000.0f) * 6.2831853f;
        if (anguloGlobal > 1.5707f && anguloGlobal < 4.7123f) {
            temaAtivoInicial = 1; 
        } else {
            temaAtivoInicial = 2; 
        }
    }
    
    auto tpAgora = std::chrono::steady_clock::now();
    std::chrono::duration<float> diffInicioInicial = tpAgora - tempoInicio;
    float tempoAbsolutoInicial = diffInicioInicial.count();

    // 1. Renderizar e preparar a textura do mapa 3D enquanto a tela esta velada
    RaycasterRenderer::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, (ALTURA_INTERNA / 2.0f), 0, profundidadeMaxima, tempoAbsolutoInicial, matrizDoMapa, tituloMapa, temaFloresta, temaAtivoInicial, cacheSprites);

    if (auto* d2d = D2DContext::renderer) {
        RaycasterD2DUtils::preencherBackbuffer(*d2d, tela3D, LARGURA_TELA, ALTURA_TELA);
        d2d->copiarBackbufferParaTextura();
    }

    // 2. Disparar o Fade In cinematografico estilo Skyrim e initialize a animacao de queda do titulo do mapa
    if (tipoAnimacaoEntrada == 1 || tipoAnimacaoEntrada == 2) {
        animacaoTituloMapaTimer = animacaoTituloMapaDuracao;
        arteTituloMapa = obterArteTituloMapaRaycaster(tituloMapa);
        if (tipoAnimacaoEntrada == 1) {
            executarAnimacaoPortaAbrindo();
        }
    }




    bool primeiraIteracaoMouse = true;
    bool rodando = true;

    while (rodando) {
        if (auto* win = D2DContext::window) {
            HWND h = win->obterHWND();
            bool isForeground = (GetForegroundWindow() == h);
            if (isForeground && !hasFocus) {
                RECT rc;
                GetWindowRect(h, &rc);
                ClipCursor(&rc);
                GameWindow::ocultarCursor();
                hasFocus = true;
            } else if (!isForeground && hasFocus) {
                ClipCursor(nullptr);
                GameWindow::mostrarCursor();
                hasFocus = false;
            }
        }

        tp2 = chrono::steady_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float tempoDelta = elapsedTime.count();
        
        chrono::duration<float> diffInicio = tp2 - tempoInicio;
        float tempoAbsoluto = diffInicio.count();

        // Limitador de delta para nao "pular" paredes ou quebrar o map se a thread travar
        if (tempoDelta > 0.1f) tempoDelta = 0.1f;

        InputSystem::Update();

        if (auto* win = D2DContext::window) {
            if (!win->processarMensagens()) {
                rodando = false;
                break;
            }
        }

        if (jogador->obterVida() <= 0 || jogador->obterVoltarProMenu()) {
            rodando = false;
            break;
        }

        bool isMoving = false;
        char acaoRetorno = RaycasterControls::processarInputEControles(
            jogador,
            jogadorX,
            jogadorY,
            anguloVisao,
            pitchOffset,
            tempoDelta,
            velocidadeMovimento,
            matrizDoMapa,
            ALTURA_TELA,
            sensibilidadeX,
            sensibilidadeY,
            primeiraIteracaoMouse,
            outHitX,
            outHitY,
            rodando,
            tp1,

            isMoving,
            bobbingTime,
            bobbingAmplitude,
            bobbingOffset
        );
        if (acaoRetorno != '\0') {
            return acaoRetorno;
        }

        // --- RENDERIZACAO RAYCASTING (3D) ---
        float horizonteInterno = (ALTURA_INTERNA / 2.0f) + (bobbingOffset * 2) + (pitchOffset * ALTURA_INTERNA);
        int offsetGeral = (bobbingOffset * 2) + (int)(pitchOffset * ALTURA_INTERNA);
        

        // --- CICLO DIA/NOITE GLOBAL ---
        int temaAtivo = temaCeu;
        if (temaCeu == 1 || temaCeu == 2) {
            long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            float anguloGlobal = ((globalMs % 60000) / 60000.0f) * 6.2831853f;
            if (anguloGlobal > 1.5707f && anguloGlobal < 4.7123f) {
                temaAtivo = 1; 
            } else {
                temaAtivo = 2; 
            }
        }

        RaycasterRenderer::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, horizonteInterno, offsetGeral, profundidadeMaxima, tempoAbsoluto, matrizDoMapa, tituloMapa, temaFloresta, temaAtivo, cacheSprites);

        if (auto* d2d = D2DContext::renderer) {
            RaycasterD2DUtils::preencherBackbuffer(*d2d, tela3D, LARGURA_TELA, ALTURA_TELA);
            d2d->copiarBackbufferParaTextura();
            
            d2d->obterRenderTarget()->BeginDraw();
            if (ID2D1Bitmap* tex = d2d->obterTexturaBackbuffer()) {
                RaycasterD2DUtils::desenharBitmapTelaCheia(*d2d, tex);
                D2D1_SIZE_F rsz = d2d->obterRenderTarget()->GetSize();
                
                RaycasterHUD::desenharProcedural(*d2d, rsz.width, rsz.height, jogadorX, jogadorY, anguloVisao, matrizDoMapa, tituloMapa, temaFloresta, jogador);
                
                if (animacaoTituloMapaTimer > 0.0f) {
                    animacaoTituloMapaTimer -= tempoDelta;
                    if (animacaoTituloMapaTimer < 0.0f) animacaoTituloMapaTimer = 0.0f;
                    
                    if (!arteTituloMapa.empty() && !arteTituloMapa[0].empty()) {
                        float t = 1.0f - (animacaoTituloMapaTimer / animacaoTituloMapaDuracao); // 0.0 to 1.0
                        float pixelScale = 8.0f;
                        int charWidth = 0;
                        for (size_t i = 0; i < arteTituloMapa[0].length(); ) {
                            if ((arteTituloMapa[0][i] & 0xC0) != 0x80) charWidth++;
                            i++;
                        }
                        
                        float pixelWidth = charWidth * pixelScale;
                        float startX = (rsz.width - pixelWidth) / 2.0f;
                        float height = arteTituloMapa.size() * (pixelScale * 1.5f);
                        float targetY = (rsz.height - height) / 2.0f;
                        
                        float currentY = 0.0f;
                        float currentAlpha = 1.0f;
                        
                        if (t < 0.3f) {
                            float progress = t / 0.3f;
                            progress = 1.0f - (1.0f - progress) * (1.0f - progress); // ease out
                            currentY = -height + (targetY + height) * progress;
                        } else if (t < 0.7f) {
                            currentY = targetY;
                        } else {
                            currentY = targetY;
                            float progress = (t - 0.7f) / 0.3f;
                            currentAlpha = 1.0f - progress;
                        }
                        
                        std::vector<GrupoCorUI> paletaArte = {
                            {"█", 255, 215, 0} // Dourado
                        };
                        
                        UIRenderer2D::DrawPixelArt(d2d, arteTituloMapa, paletaArte, startX, currentY, pixelScale, currentAlpha);
                    }
                }
            }
            d2d->obterRenderTarget()->EndDraw();
        }

        auto frameEnd = chrono::steady_clock::now();
        auto frameDuration = chrono::duration_cast<chrono::milliseconds>(frameEnd - tp2).count();
        int sleepTime = 16 - static_cast<int>(frameDuration);
        if (sleepTime > 0) {
            this_thread::sleep_for(chrono::milliseconds(sleepTime));
        }
    }

    if (hasFocus) {
        ClipCursor(nullptr);
        GameWindow::mostrarCursor();
    }

    // Ao apertar ESC, o loop morre, limpa o console e o controle volta para o game top-down padrao
    InputControl::limparBuffer();
    // So limpa a screen se o fechamento foi manual (ESC). Se bateu em entity, preserva a visao 3D como fundo pro Popup!
    if (outHitX == -1 && outHitY == -1) {
    }
    return 0;
}

std::vector<std::string> Raycaster::desenharQuadroEstatico3D(const std::vector<std::string>& matrizDoMapa, float jogadorX, float jogadorY, float anguloVisao, const std::string& tituloMapa, Character* jogador, int alturaOverride) {
    (void)jogador;
    int LARGURA_TELA = 120;
    int ALTURA_TELA = (alturaOverride > 0) ? alturaOverride : 40;
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 40;

    bool temaFloresta = RaycasterWorld::isTemaFloresta(tituloMapa);
    int temaCeu = RaycasterWorld::obterTemaCeu(tituloMapa);

    const auto& cacheSprites = RaycasterNPCs::obterCacheGlobal();

    int ALTURA_INTERNA = ALTURA_TELA;
    std::vector<Pixel3D> tela3D(LARGURA_TELA * ALTURA_INTERNA);

    RaycasterRenderer::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, (ALTURA_INTERNA / 2.0f), 0, 150.0f, 0.0f, matrizDoMapa, tituloMapa, temaFloresta, temaCeu, cacheSprites);

    std::vector<std::string> resultado(ALTURA_INTERNA * LARGURA_TELA, "");
    for (int y = 0; y < ALTURA_INTERNA; y++) {
        for (int x = 0; x < LARGURA_TELA; x++) {
            const Pixel3D& p = tela3D[y * LARGURA_TELA + x];
            resultado[y * LARGURA_TELA + x] = std::string(1, p.ch);
        }
    }

    return resultado;
}

void Raycaster::renderizarFundoAtualizadoD2D() {
    if (!s_mapaAtual || !s_currentPlayer) return;
    auto* d2d = D2DContext::renderer;
    if (!d2d) return;

    int LARGURA_TELA = 640;
    int ALTURA_TELA = 360;
    
    bool temaFloresta = RaycasterWorld::isTemaFloresta(s_tituloAtual);
    int temaCeu = RaycasterWorld::obterTemaCeu(s_tituloAtual);

    int temaAtivo = temaCeu;
    if (temaCeu == 1 || temaCeu == 2) {
        long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float anguloGlobal = ((globalMs % 60000) / 60000.0f) * 6.2831853f;
        if (anguloGlobal > 1.5707f && anguloGlobal < 4.7123f) {
            temaAtivo = 1; 
        } else {
            temaAtivo = 2; 
        }
    }

    auto tpAgora = std::chrono::steady_clock::now();
    std::chrono::duration<float> diffInicioInicial = tpAgora - s_tpInicio;
    float tempoAbsoluto = diffInicioInicial.count();

    const auto& cacheSprites = RaycasterNPCs::obterCacheGlobal();

    int ALTURA_INTERNA = ALTURA_TELA;
    std::vector<Pixel3D> tela3D(LARGURA_TELA * ALTURA_INTERNA);

    // Keep horizontal bobbing exactly where it was frozen at 0
    float horizonteInterno = (ALTURA_INTERNA / 2.0f);
    int offsetGeral = 0;
    
    RaycasterRenderer::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, s_jogadorXAtual, s_jogadorYAtual, s_anguloAtual, horizonteInterno, offsetGeral, 150.0f, tempoAbsoluto, *s_mapaAtual, s_tituloAtual, temaFloresta, temaAtivo, cacheSprites);

    RaycasterD2DUtils::preencherBackbuffer(*d2d, tela3D, LARGURA_TELA, ALTURA_TELA);
    d2d->copiarBackbufferParaTextura();
    
    d2d->obterRenderTarget()->BeginDraw();
    if (ID2D1Bitmap* tex = d2d->obterTexturaBackbuffer()) {
        RaycasterD2DUtils::desenharBitmapTelaCheia(*d2d, tex);
        D2D1_SIZE_F rsz = d2d->obterRenderTarget()->GetSize();
        RaycasterHUD::desenharProcedural(*d2d, rsz.width, rsz.height, s_jogadorXAtual, s_jogadorYAtual, s_anguloAtual, *s_mapaAtual, s_tituloAtual, temaFloresta, s_currentPlayer);
    }
}

void Raycaster::executarAnimacaoPortaAbrindo() {
    auto* d2d = D2DContext::renderer;
    if (!d2d) return;

    const int TOTAL_FRAMES = 15; // Fade-in rapido e elegante de ~0.25s a 60 FPS
    const float screenW = (float)D2DRenderer::BACKBUFFER_WIDTH;
    const float screenH = (float)D2DRenderer::BACKBUFFER_HEIGHT;

    for (int frame = 0; frame <= TOTAL_FRAMES; frame++) {
        auto tpFrameStart = std::chrono::steady_clock::now();
        float t = (float)frame / (float)TOTAL_FRAMES;
        float alphaPreto = 1.0f - t; // Transicao suave de preto total (1.0) ate visivel (0.0)

        d2d->comecarQuadro();
        d2d->limpar(D2D1::ColorF(0.0f, 0.0f, 0.0f));

        // 1. Desenhar o novo mapa 3D ao fundo
        RaycasterD2DUtils::desenharBitmapTelaCheia(*d2d, d2d->obterTexturaBackbuffer());

        // 2. Camada preta de Fade Out suave cobrindo 100% da janela para revelar a visao 3D
        if (alphaPreto > 0.001f) {
            D2D1_SIZE_F rsz = d2d->obterRenderTarget()->GetSize();
            d2d->preencherRetangulo(0, 0, rsz.width, rsz.height, D2D1::ColorF(0.0f, 0.0f, 0.0f, alphaPreto));
        }

        d2d->finalizarQuadro();

        auto tpFrameEnd = std::chrono::steady_clock::now();
        auto frameMs = std::chrono::duration_cast<std::chrono::milliseconds>(tpFrameEnd - tpFrameStart).count();
        if (frameMs < 16) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16 - frameMs));
        }
    }
}

