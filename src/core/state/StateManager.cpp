// Implementacao do gerenciador de estados do jogo.
// Controla o ciclo de vida do jogador e transicoes entre mapas mundiais.

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

// Executa o estado do menu principal.
// Exibe opcoes de criacao ou selecao de personagem e transita para ExplorationState.
void MenuState::execute(Game& game, GameContext& ctx) {
    auto activePlayer = GameMenu::mainMenu();
    if (!activePlayer) {
        game.changeState(nullptr);
        return;
    }
    ctx.playerEntity = std::move(activePlayer);
    game.changeState(std::make_unique<ExplorationState>());
}

// Limpeza de recursos ao sair do estado de exploracao de mapas.
void ExplorationState::onExit(Game& game, GameContext& ctx) {
    ctx.playerEntity.reset();
}

// Executa o loop de exploracao do mundo do jogo.
// Instancia mapas e gerencia transicoes entre zonas ativas.
void ExplorationState::execute(Game& game, GameContext& ctx) {
    Character* player = ctx.getPlayer();
    if (!player) {
        game.changeState(nullptr);
        return;
    }

    auto mapVillage = std::make_unique<Mapa1Vila>(player);
    auto mapForest = std::make_unique<Mapa2Floresta>(player);
    auto mapKingdomBridge = std::make_unique<Mapa3PonteReino>(player);
    auto mapKingdom = std::make_unique<Mapa4Reino>(player);

    IMapa* currentMap = mapVillage.get();
    while (currentMap) {
        ProximaTransicaoMapa transition = currentMap->iniciarLoopDeExploracao();

        if (transition == ProximaTransicaoMapa::VoltarMenu || player->obterVida() <= 0 || player->obterVoltarProMenu()) {
            break;
        }
        else if (transition == ProximaTransicaoMapa::Village) {
            currentMap = mapVillage.get();
            mapVillage->exploracaoEstaAtiva = true;
        }
        else if (transition == ProximaTransicaoMapa::Forest) {
            currentMap = mapForest.get();
            mapForest->exploracaoEstaAtiva = true;
            if (!Progression::instancia().obterFlag(Flags::Visitou_Floresta)) {
                Progression::instancia().definirFlag(Flags::Visitou_Floresta, true);
            }
        }
        else if (transition == ProximaTransicaoMapa::PonteReino) {
            currentMap = mapKingdomBridge.get();
            mapKingdomBridge->exploracaoEstaAtiva = true;
            if (!Progression::instancia().obterFlag(Flags::Visitou_PonteReino)) {
                Progression::instancia().definirFlag(Flags::Visitou_PonteReino, true);
            }
        }
        else if (transition == ProximaTransicaoMapa::Kingdom) {
            currentMap = mapKingdom.get();
            mapKingdom->exploracaoEstaAtiva = true;
            if (!Progression::instancia().obterFlag(Flags::Visitou_Reino)) {
                Progression::instancia().definirFlag(Flags::Visitou_Reino, true);
            }
        }
        else {
            player->definirVoltarProMenu(true);
            break;
        }
    }

    if (player->obterVida() > 0 && !player->obterVoltarProMenu()) {
        game.changeState(nullptr);
        return;
    }

    game.changeState(std::make_unique<MenuState>());
}
