#pragma once
#include <functional>

class Character;

class MapInputController {
public:
    static bool processInputAndCommands(char key, Character* player, int& nextX, int& nextY, const std::function<void()>& restoreScreen);
    static bool processarInputEComandos(char tecla, Character* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela) {
        return processInputAndCommands(tecla, jogador, proximaPosicaoX, proximaPosicaoY, restaurarTela);
    }
};

using MapInputControllera = MapInputController;
