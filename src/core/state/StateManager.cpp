// Implementation of the game state manager.
// Controls the player's lifecycle and transitions between world maps.

#include "StateManager.h"
#include "../../entities/classes/archer/Archer.h"
#include "../../entities/classes/bard/Bard.h"
#include "../../entities/classes/ClassBase.h"
#include "../../entities/classes/warrior/Warrior.h"
#include "../../entities/classes/mage/Mage.h"
#include "GameMenu.h"
#include "../../maps/village/Map1Village.h"
#include "../../maps/forest/Map2Forest.h"
#include "../../maps/kingdom/Map3KingdomBridge.h"
#include "../../maps/kingdom/Map4Kingdom.h"
#include "../../entities/races/dwarf/Dwarf.h"
#include "../../entities/races/elf/Elf.h"
#include "../../entities/races/human/Human.h"
#include "../../entities/races/orc/Orc.h"
#include "../../systems/progress/Progression.h"
#include "../../systems/progress/ProgressionFlags.h"

// Executes the main menu state.
// Displays character creation or selection options and transitions to ExplorationState.
void EstadoMenu::executar(Jogo& jogo, ContextoDoJogo& ctx) {
    auto jogadorAtivo = GameMenu::mainMenu();
    if (!jogadorAtivo) {
        jogo.mudarEstado(nullptr);
        return;
    }
    ctx.objetoJogador = std::move(jogadorAtivo);
    jogo.mudarEstado(std::make_unique<EstadoExploracao>());
}

// Resource cleanup when exiting the map exploration state.
void EstadoExploracao::aoSair(Jogo& jogo, ContextoDoJogo& ctx) {
    ctx.objetoJogador.reset();
}

// Executes the game world exploration loop.
// Instantiates maps and manages transitions between active zones.
void EstadoExploracao::executar(Jogo& jogo, ContextoDoJogo& ctx) {
    Character* jogador = ctx.obterJogador();
    if (!jogador) {
        jogo.mudarEstado(nullptr);
        return;
    }

    auto mapaVila = std::make_unique<Mapa1Vila>(jogador);
    auto mapaFloresta = std::make_unique<Mapa2Floresta>(jogador);
    auto mapaPonteReino = std::make_unique<Mapa3PonteReino>(jogador);
    auto mapaReino = std::make_unique<Mapa4Reino>(jogador);

    IMapa* mapaAtual = mapaVila.get();
    while (mapaAtual) {
        ProximaTransicaoMapa transicao = mapaAtual->iniciarLoopDeExploracao();

        if (transicao == ProximaTransicaoMapa::VoltarMenu || jogador->obterVida() <= 0 || jogador->obterVoltarProMenu()) {
            break;
        }
        else if (transicao == ProximaTransicaoMapa::Village) {
            mapaAtual = mapaVila.get();
            mapaVila->exploracaoEstaAtiva = true;
        }
        else if (transicao == ProximaTransicaoMapa::Forest) {
            mapaAtual = mapaFloresta.get();
            mapaFloresta->exploracaoEstaAtiva = true;
            if (!Progression::instancia().obterFlag(Flags::Visitou_Floresta)) {
                Progression::instancia().definirFlag(Flags::Visitou_Floresta, true);
            }
        }
        else if (transicao == ProximaTransicaoMapa::PonteReino) {
            mapaAtual = mapaPonteReino.get();
            mapaPonteReino->exploracaoEstaAtiva = true;
            if (!Progression::instancia().obterFlag(Flags::Visitou_PonteReino)) {
                Progression::instancia().definirFlag(Flags::Visitou_PonteReino, true);
            }
        }
        else if (transicao == ProximaTransicaoMapa::Kingdom) {
            mapaAtual = mapaReino.get();
            mapaReino->exploracaoEstaAtiva = true;
            if (!Progression::instancia().obterFlag(Flags::Visitou_Reino)) {
                Progression::instancia().definirFlag(Flags::Visitou_Reino, true);
            }
        }
        else {
            jogador->definirVoltarProMenu(true);
            break;
        }
    }

    if (jogador->obterVida() > 0 && !jogador->obterVoltarProMenu()) {
        jogo.mudarEstado(nullptr);
        return;
    }

    jogo.mudarEstado(std::make_unique<EstadoMenu>());
}
