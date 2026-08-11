#pragma once
#include <functional>

class Character;

class MapInputControllera {
public:
    static bool processarInputEComandos(char tecla, Character* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela);
};
