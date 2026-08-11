#pragma once
#include "../../../maps/interfaces/MapInteraction.h"

class Character;
enum class LocalizacaoMapa {
    VilaInicial,
    Forest,
    PonteReino,
    Kingdom
};

class TelaMapaMundo {
public:
    static ProximaTransicaoMapa display(Character* currentPlayer, LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino);

    // English Aliases
    static ProximaTransicaoMapa show(Character* player, LocalizacaoMapa loc, int progVila, int progForest, int progBridge, int progKingdom) {
        return display(player, loc, progVila, progForest, progBridge, progKingdom);
    }
};

using WorldMapScreen = TelaMapaMundo;
using MapLocation = LocalizacaoMapa;
