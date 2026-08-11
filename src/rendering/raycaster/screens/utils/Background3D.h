#pragma once

#include "FrameOverlayUtils.h"
#include "../../engine-raycaster/RaycasterRendererCombat.h"

namespace MenuRaycasterUtils {

    inline void cachearBackground3D(const std::string& bioma, Character* jogador) {
        if (bioma != s_ultimoBiomaMenu || s_fundo3DMenu.empty()) {
            std::string linhaVazia = "";
            for(int i = 0; i < 192; i++) linhaVazia += " ";
            s_fundo3DMenu.assign(60, linhaVazia);
            s_ultimoBiomaMenu = bioma;
        }
    }

    inline void limparBackground3D() {
        s_fundo3DMenu.clear();
        s_ultimoBiomaMenu.clear();
    }

}
