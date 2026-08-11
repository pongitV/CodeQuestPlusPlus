#include "ScreenMapWorld.h"
#include "ScreenMapWorldLayout.h"
#include "../../UIManager.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/progress/Progression.h"
#include "../../../systems/progress/ProgressionFlags.h"
#include "../../../rendering/raycaster/screens/utils/MenuD2DUtils.h"
#include "../../../rendering/direct-2d/GerenciadorTexturas2D.h"
#include "../../../rendering/direct-2d/RaycasterD2DUtils.h"
#include "../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../core/d2d-context/D2DContext.h"

ProximaTransicaoMapa TelaMapaMundo::display(Character* currentPlayer, LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino) {
    (void)currentPlayer;
    (void)progressoVila;
    (void)progressoFloresta;
    (void)progressoPonteReino;
    (void)progressoReino;

    auto locais = ArtesMapaMundo::obterLocais();
    std::vector<std::string> lugares;

    for (const auto& l : locais) {
        if (l.flag == nullptr) {
            lugares.push_back(l.nomeExibicao);
        } else if (Progression::instancia().obterFlag(l.flag)) {
            lugares.push_back(l.nomeExibicao);
        }
    }

    std::string nomeLocalAtual;
    switch (localAtual) {
        case LocalizacaoMapa::VilaInicial: nomeLocalAtual = "Village Inicial"; break;
        case LocalizacaoMapa::Forest:   nomeLocalAtual = "Forest Sombria"; break;
        case LocalizacaoMapa::PonteReino: nomeLocalAtual = "Ponte do Kingdom"; break;
        case LocalizacaoMapa::Kingdom:      nomeLocalAtual = "Kingdom Distante"; break;
    }

    int sel = 0;

    std::vector<std::string> options = {"[ VOLTAR ]"};
    options.insert(options.end(), lugares.begin(), lugares.end());

    auto renderCb = [&](UIDynamicBox& box, int selA, float optStartX, float optStartY, float pixelScale) {
        auto d2d = D2DContext::renderer;
        auto rt = d2d ? d2d->obterRenderTarget() : nullptr;
        if (d2d && rt) {
            ID2D1Bitmap* mapBitmap = GerenciadorTexturas2D::obterTextura(rt, "assets/worldMap.png");
            if (mapBitmap) {
                RaycasterD2DUtils::desenharBitmapTelaCheia(*d2d, mapBitmap, 0.85f);
            }
        }
        float lw = UIRenderer2D::LOGICAL_WIDTH;
        float lh = UIRenderer2D::LOGICAL_HEIGHT;

        float totalHeight = options.size() * 45.0f;
        float startY = (lh - totalHeight) / 2.0f;

        for (int i = 0; i < (int)options.size(); ++i) {
            std::wstring wOp(options[i].begin(), options[i].end());
            std::wstring text = (i == selA) ? (L"> " + wOp + L" <") : (L"  " + wOp + L"  ");
            D2D1_COLOR_F color = (i == selA) ? D2D1::ColorF(0.0f, 1.0f, 0.5f) : D2D1::ColorF(0.9f, 0.95f, 1.0f);
            
            box.AddText(text, lw / 2.0f, startY + i * 45.0f, 22.0f, color, true);
        }
    };

    sel = 0;
    while (true) {
        std::vector<std::string> emptyOpcoes(options.size());
        int res = MenuRaycasterUtils::renderizarMenuPadrao(
            L"MAPA MUNDI - VIAGEM RAPIDA",
            D2D1::ColorF(0.0f, 0.9f, 1.0f),
            emptyOpcoes,
            {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 8.0f,
            renderCb
        );
        
        sel = (res != -1) ? res : 0;
        char tecla = (res == -1) ? 27 : '\r';

        if (tecla == '\n' || tecla == '\r') {
            if (sel == 0) {
                return ProximaTransicaoMapa::Nenhuma;
            } else {
                std::string& nomeSel = lugares[sel - 1];
                if (nomeSel == nomeLocalAtual)
                    return ProximaTransicaoMapa::Nenhuma;
                else if (nomeSel.find("Village") != std::string::npos)
                    return ProximaTransicaoMapa::Village;
                else if (nomeSel.find("Forest") != std::string::npos)
                    return ProximaTransicaoMapa::Forest;
                else if (nomeSel.find("Ponte") != std::string::npos)
                    return ProximaTransicaoMapa::PonteReino;
                else if (nomeSel.find("Kingdom") != std::string::npos)
                    return ProximaTransicaoMapa::Kingdom;
            }
        } else if (tecla == 27) {
            return ProximaTransicaoMapa::Nenhuma;
        }
    }
    return ProximaTransicaoMapa::Nenhuma;
}
