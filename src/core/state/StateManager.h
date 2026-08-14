/**
 * @file StateManager.h
 * @brief Definição da Máquina de Estados Finitos (FSM) que controla o fluxo principal do jogo.
 * 
 * Este arquivo define a arquitetura central de estados (Padrão State), permitindo
 * transições suaves entre menus, combate, exploração e outras telas. O GameContext
 * trafega os ponteiros vitais da engine entre estes estados.
 */

#pragma once

#include <memory>
#include <mutex>
#include "../../entities/character/Character.h"

class Game;
class GameWindow;
class D2DRenderer;
class FramePipeline;
class GridEmulator2D;
class GerenciadorPerspectiva;

/**
 * @struct GameContext
 * @brief Contêiner de Injeção de Dependências que centraliza referências aos subsistemas.
 * 
 * O GameContext é passado para cada estado para garantir acesso aos recursos gráficos e
 * aos dados persistentes do jogador sem acoplamento direto ou uso excessivo de Singletons.
 */
struct GameContext {
    std::unique_ptr<Character> playerEntity;
    GameWindow* window = nullptr;
    D2DRenderer* renderer = nullptr;
    FramePipeline* pipeline = nullptr;
    GridEmulator2D* grid = nullptr;

    Character* getPlayer() { return playerEntity.get(); }
    const Character* getPlayer() const { return playerEntity.get(); }
    Character* player() { return getPlayer(); }
    const Character* player() const { return getPlayer(); }

    void setWindow(GameWindow* win) { window = win; }
    GameWindow* getWindow() const { return window; }
    bool hasWindow() const { return window != nullptr; }

    void setRenderer(D2DRenderer* rnd) { renderer = rnd; }
    D2DRenderer* getRenderer() const { return renderer; }
    bool hasRenderer() const { return renderer != nullptr; }

    void setPipeline(FramePipeline* pipe) { pipeline = pipe; }
    FramePipeline* getPipeline() const { return pipeline; }
    bool hasPipeline() const { return pipeline != nullptr; }

    void setGrid(GridEmulator2D* gr) { grid = gr; }
    GridEmulator2D* getGrid() const { return grid; }
    bool hasGrid() const { return grid != nullptr; }

    // Compatibilidade e delegações legadas
    Character* obterJogador() { return getPlayer(); }
    const Character* obterJogador() const { return getPlayer(); }
    void definirJanela(GameWindow* win) { setWindow(win); }
    GameWindow* obterJanela() const { return getWindow(); }
    bool possuiJanela() const { return hasWindow(); }
    void definirRenderizador(D2DRenderer* rnd) { setRenderer(rnd); }
    D2DRenderer* obterRenderizador() const { return getRenderer(); }
    bool possuiRenderizador() const { return hasRenderer(); }
    void definirPipeline(FramePipeline* pipe) { setPipeline(pipe); }
    FramePipeline* obterPipeline() const { return getPipeline(); }
    bool possuiPipeline() const { return hasPipeline(); }
    void definirGrade(GridEmulator2D* gr) { setGrid(gr); }
    GridEmulator2D* obterGrade() const { return getGrid(); }
    bool possuiGrade() const { return hasGrid(); }

    // Aliases para membros acessados diretamente no código legado
    std::unique_ptr<Character>& objetoJogador = playerEntity;
    GameWindow*& janela = window;
    D2DRenderer*& renderizador = renderer;
    GridEmulator2D*& grade = grid;
};

using ContextoDoJogo = GameContext;
using ContextoJogo = GameContext;

/**
 * @class GameState
 * @brief Interface base para todos os estados da máquina de estados do jogo.
 */
class GameState {
public:
    virtual ~GameState() = default;

    /**
     * @brief Gatilho disparado uma vez ao entrar no estado.
     */
    virtual void onEnter(Game& game, GameContext& ctx) {}

    /**
     * @brief Função de atualização contínua, chamada a cada quadro (Game Loop).
     */
    virtual void execute(Game& game, GameContext& ctx) = 0;

    /**
     * @brief Gatilho disparado uma vez ao sair do estado.
     */
    virtual void onExit(Game& game, GameContext& ctx) {}

    // Compatibilidade legada
    virtual void aoEntrar(Game& game, GameContext& ctx) { onEnter(game, ctx); }
    virtual void executar(Game& game, GameContext& ctx) { execute(game, ctx); }
    virtual void aoSair(Game& game, GameContext& ctx) { onExit(game, ctx); }
};

using EstadoDoJogo = GameState;
using EstadoJogo = GameState;

/**
 * @class Game
 * @brief Orquestrador principal da Máquina de Estados (Contexto do Padrão State).
 */
class Game {
private:
    std::unique_ptr<GameState> currentState;
    std::unique_ptr<GameState> nextState;
    bool pendingChange = false; // Garante que a transição ocorra de modo atômico fora do frame
    GameContext context;

public:
    explicit Game(std::unique_ptr<GameState> initialState) noexcept
        : currentState(std::move(initialState)) {}

    void changeState(std::unique_ptr<GameState> newState) noexcept {
        nextState = std::move(newState);
        pendingChange = true;
    }

    void mudarEstado(std::unique_ptr<GameState> novoEstado) noexcept { changeState(std::move(novoEstado)); }

    GameContext& getContext() noexcept { return context; }
    const GameContext& getContext() const noexcept { return context; }
    GameContext& obterContexto() noexcept { return getContext(); }
    const GameContext& obterContexto() const noexcept { return getContext(); }

    void runLoop() {
        if (currentState) currentState->onEnter(*this, context);
        while (currentState) {
            currentState->execute(*this, context);

            if (pendingChange) {
                if (currentState) currentState->onExit(*this, context);
                currentState = std::move(nextState);
                if (currentState) currentState->onEnter(*this, context);
                pendingChange = false;
            }
        }
    }

    void executarLoop() { runLoop(); }
    void rodar() { runLoop(); }
    void run() { runLoop(); }
};

using Jogo = Game;

// Estado de exploração do mapa mundo
class ExplorationState final : public GameState {
public:
    void execute(Game& game, GameContext& ctx) override;
    void onExit(Game& game, GameContext& ctx) override;

    void executar(Game& game, GameContext& ctx) override { execute(game, ctx); }
    void aoSair(Game& game, GameContext& ctx) override { onExit(game, ctx); }
};

using EstadoExploracao = ExplorationState;

// Estado do menu principal do jogo
class MenuState final : public GameState {
public:
    void execute(Game& game, GameContext& ctx) override;
    void executar(Game& game, GameContext& ctx) override { execute(game, ctx); }
};

using EstadoMenu = MenuState;
