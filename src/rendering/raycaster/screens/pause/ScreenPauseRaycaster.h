#pragma once

#include <string>
#include <vector>

class Character;

class TelaPauseRaycaster {
public:
    static int renderizarMenuPause();
    static int renderizarMenuConfiguracoes(Character* jogador);
    static int renderizarMenuAparencia(Character* jogador);
    static int renderizarMenuFundo(int corFundoAtualIndex);
    static int renderizarMenuSensibilidade(int percX, int percY);
};
