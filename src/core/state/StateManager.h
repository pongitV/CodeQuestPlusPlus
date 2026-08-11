/**
 * @file StateManager.h
 * @brief Definition of the Finite State Machine (FSM) that controls the main game flow.
 * 
 * This file defines the central state architecture (State Pattern), allowing
 * smooth transitions between menus, combat, exploration, and other screens. The GameContext
 * passes the vital engine pointers between these states.
 */

#pragma once

#include <memory>
#include <mutex>
#include "../../entities/character/Character.h"

class Jogo;
class GameWindow;
class D2DRenderer;
class FramePipeline;
class GridEmulator2D;
class GerenciadorDePerspectiva;

/**
 * @struct ContextoDoJogo
 * @brief Dependency Injection Container that centralizes subsystem references.
 * 
 * The ContextoDoJogo is passed to each state to ensure access to graphics resources and
 * player persistent data without direct coupling or excessive use of Singletons.
 * - `objetoJogador`: Unique owner (unique_ptr) of the player entity, keeping the state alive between screens.
 * - `janela`, `renderizador`, etc.: Observation pointers (Views) managed by `main.cpp`.
 */
struct ContextoDoJogo {
    std::unique_ptr<Character> objetoJogador;
    GameWindow* janela = nullptr;
    D2DRenderer* renderizador = nullptr;
    FramePipeline* pipeline = nullptr;
    GridEmulator2D* grade = nullptr;

    Character* obterJogador() { return objetoJogador.get(); }
    const Character* obterJogador() const { return objetoJogador.get(); }
    Character* player() { return obterJogador(); }
    const Character* player() const { return obterJogador(); }

    void definirJanela(GameWindow* win) { janela = win; }
    GameWindow* obterJanela() const { return janela; }
    bool possuiJanela() const { return janela != nullptr; }
    void setWindow(GameWindow* win) { definirJanela(win); }
    GameWindow* getWindow() const { return obterJanela(); }
    bool hasWindow() const { return possuiJanela(); }

    void definirRenderizador(D2DRenderer* rnd) { renderizador = rnd; }
    D2DRenderer* obterRenderizador() const { return renderizador; }
    bool possuiRenderizador() const { return renderizador != nullptr; }
    void setRenderer(D2DRenderer* rnd) { definirRenderizador(rnd); }
    D2DRenderer* getRenderer() const { return obterRenderizador(); }
    bool hasRenderer() const { return possuiRenderizador(); }

    void definirPipeline(FramePipeline* pipe) { pipeline = pipe; }
    FramePipeline* obterPipeline() const { return pipeline; }
    bool possuiPipeline() const { return pipeline != nullptr; }
    void setPipeline(FramePipeline* pipe) { definirPipeline(pipe); }
    FramePipeline* getPipeline() const { return obterPipeline(); }
    bool hasPipeline() const { return possuiPipeline(); }

    void definirGrade(GridEmulator2D* gr) { grade = gr; }
    GridEmulator2D* obterGrade() const { return grade; }
    bool possuiGrade() const { return grade != nullptr; }
    void setGrid(GridEmulator2D* gr) { definirGrade(gr); }
    GridEmulator2D* getGrid() const { return obterGrade(); }
    bool hasGrid() const { return possuiGrade(); }
};

using ContextoJogo = ContextoDoJogo;
using GameContext = ContextoDoJogo;

/**
 * @class EstadoDoJogo
 * @brief Base interface for all game state machine states.
 * 
 * Implements the "State" design pattern. Each screen or main game mode must
 * derive from this class and implement execution logic.
 */
class EstadoDoJogo {
public:
    virtual ~EstadoDoJogo() = default;

    /**
     * @brief Trigger fired once when entering this state.
     */
    virtual void aoEntrar(Jogo& jogo, ContextoDoJogo& ctx) {}

    /**
     * @brief Continuous update function, called each frame (Game Loop).
     * @param jogo Reference to the orchestrator (FSM) to allow state switching.
     * @param ctx Reference to the context to access rendering resources and player.
     */
    virtual void executar(Jogo& jogo, ContextoDoJogo& ctx) = 0;

    /**
     * @brief Trigger fired once when exiting this state (cleanup of temporary resources).
     */
    virtual void aoSair(Jogo& jogo, ContextoDoJogo& ctx) {}

    void onEnter(Jogo& jogo, ContextoDoJogo& ctx) { aoEntrar(jogo, ctx); }
    void execute(Jogo& jogo, ContextoDoJogo& ctx) { executar(jogo, ctx); }
    void onExit(Jogo& jogo, ContextoDoJogo& ctx) { aoSair(jogo, ctx); }
};

using EstadoJogo = EstadoDoJogo;
using GameState = EstadoDoJogo;

/**
 * @class Jogo
 * @brief Main orchestrator of the State Machine (Context of the State Pattern).
 * 
 * Manages ownership of the current state, queues state changes safely, and 
 * keeps the `ContextoDoJogo` alive, propagating it in the Game Loop.
 */
class Jogo {
private:
    std::unique_ptr<EstadoDoJogo> estadoAtual;
    std::unique_ptr<EstadoDoJogo> proximoEstado;
    bool mudancaPendente = false; // Flag to ensure the transition occurs atomically outside the middle of the frame.
    ContextoDoJogo contexto;

public:
    explicit Jogo(std::unique_ptr<EstadoDoJogo> estadoInicial) noexcept
        : estadoAtual(std::move(estadoInicial)) {}

    void mudarEstado(std::unique_ptr<EstadoDoJogo> novoEstado) noexcept {
        proximoEstado = std::move(novoEstado);
        mudancaPendente = true;
    }

    void changeState(std::unique_ptr<EstadoDoJogo> newState) noexcept { mudarEstado(std::move(newState)); }

    ContextoDoJogo& obterContexto() noexcept { return contexto; }
    const ContextoDoJogo& obterContexto() const noexcept { return contexto; }
    ContextoDoJogo& getContext() noexcept { return obterContexto(); }
    const ContextoDoJogo& getContext() const noexcept { return obterContexto(); }

    void executarLoop() {
        if (estadoAtual) estadoAtual->aoEntrar(*this, contexto);
        while (estadoAtual) {
            estadoAtual->executar(*this, contexto);

            if (mudancaPendente) {
                if (estadoAtual) estadoAtual->aoSair(*this, contexto);
                estadoAtual = std::move(proximoEstado);
                if (estadoAtual) estadoAtual->aoEntrar(*this, contexto);
                mudancaPendente = false;
            }
        }
    }

    void rodar() { executarLoop(); }
    void run() { executarLoop(); }
};

using Game = Jogo;

// Map world exploration state.
class EstadoExploracao final : public EstadoDoJogo {
public:
    void executar(Jogo& jogo, ContextoDoJogo& ctx) override;
    void aoSair(Jogo& jogo, ContextoDoJogo& ctx) override;

    void execute(Jogo& jogo, ContextoDoJogo& ctx) { executar(jogo, ctx); }
    void onExit(Jogo& jogo, ContextoDoJogo& ctx) { aoSair(jogo, ctx); }
};

using ExplorationState = EstadoExploracao;

// Main game menu state.
class EstadoMenu final : public EstadoDoJogo {
public:
    void executar(Jogo& jogo, ContextoDoJogo& ctx) override;

    void execute(Jogo& jogo, ContextoDoJogo& ctx) { executar(jogo, ctx); }
};

using MenuState = EstadoMenu;
