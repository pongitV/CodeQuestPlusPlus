#include "ScreenPauseRaycaster.h"
#include "../utils/MenuRaycasterUtils.h"
#include "../utils/MenuD2DUtils.h"
#include "../utils/MenuRaycasterLayout.h"
#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../entities/character/Character.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/window/GameWindow.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>

namespace {
    using MenuRaycasterUtils::toWStringClean;
    
    std::vector<GrupoCorUI> paletaTitulo = {
        {"_", 255, 215, 0},
        {"|", 255, 215, 0},
        {"-", 255, 215, 0}
    };
}

static int renderizarMenuSimplesD2D(const std::wstring& tituloCaixa, const std::vector<std::string>& opcoes) {
    auto construtor = [&](UIDynamicBox& box, int selecaoAtual, float logicalW, float logicalH) {
        box.AddText(tituloCaixa, logicalW / 2.0f, 30.0f, 22.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);

        float opY = 120.0f;
        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::wstring wOp = toWStringClean(opcoes[i]);
            std::wstring text = (i == selecaoAtual) ? (L">" + wOp) : (L" " + wOp);
            D2D1_COLOR_F color = (i == selecaoAtual) ? D2D1::ColorF(0.2f, 1.0f, 0.2f) : D2D1::ColorF(0.6f, 0.6f, 0.6f);
            box.AddText(text, logicalW / 2.0f, opY, 18.0f, color, true);
            opY += 50.0f;
        }
    };
    
    return MenuRaycasterUtils::renderizarPopupCaixa({}, {}, (int)opcoes.size(), construtor);
}

int TelaPauseRaycaster::renderizarMenuPause() {
    std::vector<std::string> opcoes = {
        "VOLTAR AO JOGO",
        "CONFIGURACOES",
        "SAIR DO JOGO"
    };
    int res = renderizarMenuSimplesD2D(L"[ PAUSE ]", opcoes);
    if (res == -1) return 0; 
    return res;
}

int TelaPauseRaycaster::renderizarMenuConfiguracoes(Character* jogador) {
    std::string difStr;
    switch (jogador->obterDificuldade()) {
        case GameDifficulty::Easy: difStr = "FACIL"; break;
        case GameDifficulty::Normal: difStr = "NORMAL"; break;
        case GameDifficulty::Hard: difStr = "DIFICIL"; break;
    }

    std::string parryState = jogador->obterParryAtivado() ? "LIGADO" : "DESLIGADO"; 
    std::string parryType = jogador->obterParryModerno() ? "MOVIMENTO" : "DIGITACAO"; 

    std::vector<std::string> opcoes = {
        "DIFICULDADE: " + difStr,
        "PARRY: " + parryState,
        "TIPO DE PARRY: " + parryType,
        "SENSIBILIDADE DA CAMERA",
        "VOLTAR"
    };
    int res = renderizarMenuSimplesD2D(L"[ CONFIGURACOES ]", opcoes);
    
    if (res == -1) return 4;
    return res;
}

int TelaPauseRaycaster::renderizarMenuAparencia(Character*) { 
    std::vector<std::string> opcoes = {
        "MUDAR COR",
        "MUDAR ICONE",
        "VOLTAR"
    };
    int res = renderizarMenuSimplesD2D(L"[ APARENCIA ]", opcoes);
    if (res == -1) return 2;
    return res;
}

int TelaPauseRaycaster::renderizarMenuFundo(int) { 
    std::vector<std::string> opcoes = {
        "BLACK",
        "GRAY ESCURO",
        "BLUE ESCURO",
        "RED ESCURO",
        "GREEN ESCURO",
        "PURPLE ESCURO",
        "VOLTAR"
    };
    int res = renderizarMenuSimplesD2D(L"[ COR DE FUNDO ]", opcoes);
    if (res == -1) return 6;
    return res;
}

int TelaPauseRaycaster::renderizarMenuSensibilidade(int percX, int percY) {
    std::vector<std::string> opcoes = {
        "EIXO X (Yaw): " + std::to_string(percX) + "%",
        "EIXO Y (Pitch): " + std::to_string(percY) + "%",
        "VOLTAR"
    };
    int res = renderizarMenuSimplesD2D(L"[ SENSIBILIDADE ]", opcoes);
    if (res == -1) return 2;
    return res;
}
