#include "ScreenIntroductionRaycaster.h"
#include "../utils/MenuRaycasterLayout.h"
#include "../utils/MenuRaycasterUtils.h"
#include <sstream>
#include <thread>
#include <chrono>

#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../core/utils/DialogFunctions.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../core/window/GameWindow.h"
#include <windows.h>

namespace {
    using MenuRaycasterUtils::toWStringClean;
}

void TelaIntroducaoRaycaster::display() {
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

    std::string titulo = "Bem-vindo, Jovem Aventureiro!";
    std::string subtitulo = "A Jornada comeca agora...";
    const auto& cena = ArtesRaycaster::cenaIntroducao;

    std::vector<std::string> mensagens = {
        "Voce desperta nos arredores de um lugar desconhecido...",
        "Na sua vista, uma pequena vila sendo atacada por monstros.",
        "Empunhando seu equipamento, voce sente que seu destino o aguarda.",
        "Um novo capitulo se inicia agora.",
        "",
        "Pressione Enter para iniciar sua jornada."
    };

    int caracteresRevelados = 0;
    InputControl::limparBuffer();

    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        char tecla = InputControl::lerTecla();

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        MenuRaycasterUtils::desenharFundoNativoD2D(/*abrirFrame=*/true);

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            float cx = UIRenderer2D::LOGICAL_WIDTH / 2.0f;
            float cy = UIRenderer2D::LOGICAL_HEIGHT / 2.0f;

            float logoY = 60.0f;
            float pixelScale = 8.0f;
            
            auto countUtf8Chars = [](const std::string& str) {
                int count = 0;
                for (size_t lx = 0; lx < str.size(); ) {
                    unsigned char c = str[lx];
                    int charLen = 1;
                    if ((c & 0x80) == 0) charLen = 1;
                    else if ((c & 0xE0) == 0xC0) charLen = 2;
                    else if ((c & 0xF0) == 0xE0) charLen = 3;
                    else if ((c & 0xF8) == 0xF0) charLen = 4;
                    lx += charLen;
                    count++;
                }
                return count;
            };

            int cenaCharWidth = countUtf8Chars(cena[0]);
            float cenaPixelWidth = cenaCharWidth * pixelScale;
            float startCenaX = cx - (cenaPixelWidth / 2.0f);

            std::vector<GrupoCorUI> paletaCena = {
                {"█░", 255, 255, 255}
            };
            UIRenderer2D::DrawPixelArt(d2d, cena, paletaCena, startCenaX, logoY, pixelScale, 1.0f);

            UIDynamicBox box;
            float msgY = logoY + (cena.size() * (pixelScale * 1.5f)) + 20.0f + 20.0f; // + padding

            int charCount = 0;
            float currentY = msgY;
            for (const auto& msg : mensagens) {
                if (msg.empty()) {
                    currentY += 40.0f;
                    charCount += 5;
                    continue;
                }
                
                // Add invisible full text to guarantee stable box size
                box.AddText(toWStringClean(msg), cx, currentY, 16.0f, D2D1::ColorF(1,1,1), true, false);

                if (charCount >= caracteresRevelados) {
                    currentY += 30.0f;
                    continue;
                }
                
                std::string textoAtivo = "";
                for (char c : msg) {
                    if (charCount < caracteresRevelados) {
                        textoAtivo += c;
                        charCount++;
                    } else {
                        break;
                    }
                }

                if (textoAtivo == msg) {
                    charCount += 15;
                }
                
                std::wstring wTexto = toWStringClean(textoAtivo);
                box.AddText(wTexto, cx, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f), true, true);
                currentY += 30.0f;
            }

            box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, cx);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        caracteresRevelados += 2; // Speed of typing

        if (tecla == '\r' || tecla == '\n' || tecla == 27) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
