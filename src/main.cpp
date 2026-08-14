/**
 * @file main.cpp
 * @brief Ponto de entrada (Entrypoint) principal do jogo CodeQuestPlusPlus.
 * 
 * Este arquivo orquestra a inicialização da engine, instanciando os sistemas
 * vitais (Janela, Direct2D, Pipeline de Renderização, GridEmulator e Input).
 * Ele define o ContextoDoJogo que será propagado por toda a arquitetura de
 * máquina de estados.
 */

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include <memory>

#include "core/d2d-context/D2DContext.h"
#include "core/window/GameWindow.h"
#include "rendering/direct-2d/D2DRenderer.h"
#include "rendering/direct-2d/FramePipeline.h"
#include "rendering/direct-2d/GridEmulator2D.h"
#include "core/input/InputSystem.h"
#include "core/state/StateManager.h"
#include "ui/UIManager.h"

static std::unique_ptr<GameWindow> g_window;
static std::unique_ptr<D2DRenderer> g_d2d;
static std::unique_ptr<FramePipeline> g_pipeline;
static std::unique_ptr<GridEmulator2D> g_grid;
static std::unique_ptr<GameContext> g_context;

/**
 * @brief Função principal (Entrypoint do Windows).
 * 
 * @param hInstance Identificador (Handle) da instância atual da aplicação.
 * @param hPrevInstance Não utilizado no Win32 moderno.
 * @param pCmdLine Argumentos de linha de comando.
 * @param nCmdShow Sinalizador de exibição inicial da janela.
 * @return int Código de saída do processo (0 para sucesso, negativo para erro).
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)pCmdLine;

    // Inicializa o contexto global compartilhado por todos os estados do jogo.
    g_context = std::make_unique<GameContext>();

    // Inicialização do sistema de janelas do SO
    g_window = std::make_unique<GameWindow>(hInstance, nCmdShow);
    if (!g_window->getHWND()) return -1;
    g_context->setWindow(g_window.get());

    g_d2d = std::make_unique<D2DRenderer>();
    if (!g_d2d->initialize(g_window->getHWND())) return -1;
    g_context->setRenderer(g_d2d.get());

    g_grid = std::make_unique<GridEmulator2D>(*g_d2d);
    g_pipeline = std::make_unique<FramePipeline>();
    g_context->setGrid(g_grid.get());
    g_context->setPipeline(g_pipeline.get());

    g_grid->setCellSize(14.0f);
    g_grid->setGridDimensions(80, 40);

    InputSystem::Initialize(g_window->getHWND());

    D2DContext::renderer = g_d2d.get();
    D2DContext::grid = g_grid.get();
    D2DContext::pipeline = g_pipeline.get();
    D2DContext::window = g_window.get();

    PerspectiveManager::getInstance().initialize();

    // Renderiza um primeiro quadro limpo (fundo escuro) para evitar "flicker" branco
    // da janela do Windows antes que o loop principal do jogo assuma o controle.
    {
        auto* renderTarget = g_d2d->getRenderTarget();
        if (renderTarget) {
            renderTarget->BeginDraw();
            renderTarget->Clear(D2D1::ColorF(0.05f, 0.05f, 0.08f));
            renderTarget->EndDraw();
        }
    }

    // Instancia a Máquina de Estados Finita (FSM) começando no Menu Principal e dispara o loop de atualização.
    Game game(std::make_unique<MenuState>());
    game.runLoop();

    return 0;
}
