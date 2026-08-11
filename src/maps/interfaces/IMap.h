#pragma once

#include <functional>
#include <string>

// Enum representing target transitions when transitioning between game world maps.
enum class NextMapTransition {
    None = 0,
    ReturnToMenu = 1,
    Village = 2,
    Forest = 3,
    KingdomBridge = 4,
    Kingdom = 5,

    // Backward-compatibility enum constants
    Nenhuma = None,
    VoltarMenu = ReturnToMenu,
    PonteReino = KingdomBridge
};

// Legacy alias for map transitions
using ProximaTransicaoMapa = NextMapTransition;

// Interface contract for all concrete game map instances.
class IMap {
public:
    virtual ~IMap() = default;

    // Returns the map title for display and logs.
    virtual std::string getTitle() const = 0;

    // Starts and manages the active exploration loop for this map.
    virtual NextMapTransition startExplorationLoop() = 0;

    // Legacy backward-compatibility method signatures
    virtual std::string obterTitulo() const { return getTitle(); }
    virtual NextMapTransition iniciarLoopDeExploracao() { return startExplorationLoop(); }
};

// Legacy alias for map interface
using IMapa = IMap;

class Mapa2Floresta;

// Context passed into forest environmental interaction handlers.
struct ForestInteractionContext {
    Mapa2Floresta* self;
    int proximaPosicaoX;
    int proximaPosicaoY;
    int larguraDoTerminal;
    const std::function<void()>& restaurarTela;
    char celula;
    const std::function<void()>& animarTela;
};

// Legacy alias for forest interaction context
using ContextoInteracaoFloresta = ForestInteractionContext;

// Strategy pattern interface for forest interactions.
class ForestInteraction {
public:
    virtual ~ForestInteraction() = default;
    virtual void process(ForestInteractionContext& ctx) { processar(ctx); }
    virtual void processar(ForestInteractionContext& ctx) { process(ctx); }
};

// Legacy alias for forest interaction strategy
using InteracaoFloresta = ForestInteraction;

class Mapa1Vila;

// Context passed into village environmental interaction handlers.
struct VillageInteractionContext {
    Mapa1Vila* self;
    int proximaPosicaoX;
    int proximaPosicaoY;
    int larguraDoTerminal;
    const std::function<void()>& restaurarTela;
    char celula;
    const std::function<void()>& animarTela;
};

// Legacy alias for village interaction context
using ContextoInteracaoVila = VillageInteractionContext;

// Strategy pattern interface for village interactions.
class VillageInteraction {
public:
    virtual ~VillageInteraction() = default;
    virtual void process(VillageInteractionContext& ctx) { processar(ctx); }
    virtual void processar(VillageInteractionContext& ctx) { process(ctx); }
};

// Legacy alias for village interaction strategy
using InteracaoVila = VillageInteraction;
