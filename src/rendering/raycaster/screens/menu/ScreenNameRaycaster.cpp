#include "ScreenNameRaycaster.h"
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

TelaNome::Resultado TelaNomeRaycaster::display() {
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

    std::string nome;
    std::string mensagemErro = "";

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

            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            std::string promptStr = "> Digite o nome do seu character [0 para voltar]: ";
            std::string displayStr = promptStr + nome + "_";
            float textY = logicalH / 2.0f - 100.0f;
            
            UIDynamicBox box;
            box.AddText(L"O NOME DO DESTINO", logicalW / 2.0f, textY, 22.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);
            
            textY += 60.0f;
            box.AddText(L"O mundo clama por um novo destino...", logicalW / 2.0f, textY, 16.0f, D2D1::ColorF(0.7f, 0.7f, 1.0f), true);
            textY += 30.0f;
            box.AddText(L"E todas as lendas possuem um nome.", logicalW / 2.0f, textY, 16.0f, D2D1::ColorF(0.7f, 0.7f, 1.0f), true);
            
            textY += 60.0f;
            std::wstring wDisplay(displayStr.begin(), displayStr.end());
            box.AddText(wDisplay, logicalW / 2.0f, textY, 18.0f, D2D1::ColorF(0.4f, 20.0f, 0.4f), true);

            if (!mensagemErro.empty()) {
                textY += 50.0f;
                std::wstring wErr(mensagemErro.begin(), mensagemErro.end());
                box.AddText(wErr, logicalW / 2.0f, textY, 16.0f, D2D1::ColorF(1.0f, 0.4f, 0.4f), true);
            }

            box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, logicalW / 2.0f);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        mensagemErro = "";

        if (tecla == '\r' || tecla == '\n') {
            if (nome == "0") {
                TelaNome::Resultado r;
                r.voltou = true;
                return r;
            }
            if (nome.empty() || nome.length() > 20) {
                mensagemErro = nome.empty() ? "Nome invalido! Nao pode ser vazio." : "Nome muito longo! Maximo 20 caracteres.";
                continue;
            }
            TelaNome::Resultado r;
            r.nome = nome;
            return r;
        }
        if ((tecla == 8 || tecla == 127) && !nome.empty()) nome.pop_back();
        if (tecla >= 32 && tecla <= 126 && nome.length() < 20) nome += static_cast<char>(tecla);
    }
    
    TelaNome::Resultado r_fallback;
    r_fallback.voltou = true;
    return r_fallback;
}

