#pragma once
#include "../../entities/character/Character.h"

class IPauseUI {
public:
    virtual ~IPauseUI() = default;
    virtual int renderizarMenuPause() = 0;
    virtual int renderizarMenuConfiguracoes(Character* jogador) = 0;
    virtual int renderizarMenuAparencia(Character* jogador) = 0;
    virtual int renderizarMenuFundo(int corFundoAtualIndex) = 0;
    virtual int renderizarMenuSensibilidade(int percX, int percY) = 0;
};
