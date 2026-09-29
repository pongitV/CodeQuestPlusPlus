#include "ScreenDiaryRaycaster.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "../../../../ui/screens/ScreenBase.h"
#include "../../../../ui/screens/diary/ScreenDiaryLayout.h"
#include "../../engine-raycaster/Raycaster.h"
#include "../../../../core/utils/Color.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../core/utils/InputControl.h"
#include "../../engine-raycaster/RaycasterFrame.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../utils/MenuD2DUtils.h"
#include "../../../../ui/UIManager.h"
#include "../../../../core/window/GameWindow.h"
#include <d2d1.h>

void TelaDiarioRaycaster::renderizarFundo() {
    Raycaster::restaurarUltimoQuadro();
}

void TelaDiarioRaycaster::displayCabecalho(int startY) {
    int larguraConsole = 120;
    int logoHeight = ArtesDiario::logoDiario.size();
    int logoY = startY > 0 ? (startY - 1 - logoHeight) : 2;
    if (logoY < 0) logoY = 0;

    int compVisualLogo = 0;
    for (const auto& linha : ArtesDiario::logoDiario) {
        int comp = (int)(linha).length();
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = (larguraConsole - compVisualLogo) / 2;
    if (logoX < 0) logoX = 0;

    std::string corTitulo = "";
    for (int i = 0; i < (int)ArtesDiario::logoDiario.size(); ++i) {
        const std::string& linha = ArtesDiario::logoDiario[i];

        std::string buffer = linha;
    }
}

static void DesenharFundoD2D() {
    auto d2d = D2DContext::renderer;
    if (!d2d) return;
    auto rt = d2d->obterRenderTarget();
    if (!rt) return;

    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0, 0));
        auto tex = d2d->obterTexturaBackbuffer();
        if (tex) {
            D2D1_SIZE_U tsz = tex->GetPixelSize();
            D2D1_SIZE_F rsz = rt->GetSize();
            rt->DrawBitmap(tex, D2D1::RectF(0, 0, rsz.width, rsz.height), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
        }
    } else {
        MenuRaycasterUtils::desenharFundoNativoD2D(true);
        rt->BeginDraw(); 
    }
}

static void ProcessarApresentarD2D() {
    if (auto* win = D2DContext::window) {
        win->processarMensagens();
    }
    InputControl::atualizarTeclas();
    if (auto d2d = D2DContext::renderer) {
        auto rt = d2d->obterRenderTarget();
        if (rt) {
            rt->EndDraw();
        }
    }
}

void TelaDiarioRaycaster::renderizarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, Color corCaixa, int minY, int startYOverride) {
    DesenharFundoD2D();
    auto d2d = D2DContext::renderer;
    if (d2d && d2d->obterRenderTarget()) {
        auto tam = d2d->obterRenderTarget()->GetSize();
        UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);
        float logicalW = UIRenderer2D::LOGICAL_WIDTH;
        float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

        UIDynamicBox box;
        std::wstring wTit = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(titulo));
        box.SetTitle(L"", D2D1::ColorF(1.0f, 0.6f, 0.0f)); // Laranja
        box.SetTitleAscii(ArtesDiario::logoDiario, {}, 3.0f);
        
        float totalHeight = 40.0f + linhas.size() * 30.0f;
        float y = (logicalH - totalHeight) / 2.0f;
        
        // y += 40.0f; // SetTitle desenha o titulo dentro da borda nativamente.
        for (const auto& l : linhas) {
            std::string plain = MenuRaycasterUtils::stripAnsi(l);
            if (plain.length() >= 5 && plain.substr(0, 5) == "[OPC]") {
                std::string content = plain.substr(8); // Formato de exibicao da opcao selecionada ou nao selecionada
                std::wstring wContent = MenuRaycasterUtils::utf8_to_wstring(content);
                bool isSel = (plain.substr(5, 3) == " > ");
                MenuRaycasterUtils::adicionarOpcaoMenu(box, wContent, logicalW / 2.0f, y, isSel, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
            } else {
                std::wstring wText = MenuRaycasterUtils::utf8_to_wstring(plain.substr(0, plain.find_last_not_of(' ') + 1));
                box.AddText(wText, logicalW / 2.0f, y, 16.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
            }
            y += 30.0f;
        }
        // Cor da borda como reserva, SetTitle a sobrescreve
        box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.9f, 2.0f, D2D1::ColorF(1.0f, 0.6f, 0.0f), 20.0f, logicalW / 2.0f, logicalH / 2.0f);
        UIRenderer2D::ResetTransform(d2d);
    }
    ProcessarApresentarD2D();
}

void TelaDiarioRaycaster::renderizarPopupMensagem(const std::string& titulo, const std::vector<std::string>& texto) {
    DesenharFundoD2D();
    auto d2d = D2DContext::renderer;
    if (d2d && d2d->obterRenderTarget()) {
        auto tam = d2d->obterRenderTarget()->GetSize();
        UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);
        float logicalW = UIRenderer2D::LOGICAL_WIDTH;
        float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

        UIDynamicBox box;
        std::wstring wTit = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(titulo));
        box.SetTitle(wTit, D2D1::ColorF(1.0f, 0.6f, 0.0f));
        
        float totalHeight = 40.0f + texto.size() * 20.0f + 20.0f;
        float y = (logicalH - totalHeight) / 2.0f;
        
        for (const auto& t : texto) {
            box.AddText(MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(t)), logicalW / 2.0f, y, 16.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
            y += 20.0f;
        }
        y += 20.0f;
        box.AddText(L"   > Voltar", logicalW / 2.0f, y, 18.0f, D2D1::ColorF(1.0f, 1.0f, 0.0f), true);
        
        box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.9f, 2.0f, D2D1::ColorF(1.0f, 0.6f, 0.0f), 20.0f, logicalW / 2.0f, logicalH / 2.0f);
        UIRenderer2D::ResetTransform(d2d);
    }
    ProcessarApresentarD2D();
}

void TelaDiarioRaycaster::renderizarPopupInspecaoComArte(const std::string& titulo, const std::vector<std::string>& arte, const std::vector<std::string>& info, const std::string& subtitulo) {
    DesenharFundoD2D();
    auto d2d = D2DContext::renderer;
    if (d2d && d2d->obterRenderTarget()) {
        auto tam = d2d->obterRenderTarget()->GetSize();
        UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);
        float logicalW = UIRenderer2D::LOGICAL_WIDTH;
        float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

        UIDynamicBox box;
        std::wstring wTit = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(titulo));
        box.SetTitle(wTit, D2D1::ColorF(1.0f, 0.6f, 0.0f));
        
        int maxLen = std::max(arte.size(), info.size());
        
        float totalHeight = 40.0f;
        if (!subtitulo.empty()) totalHeight += 30.0f;
        totalHeight += maxLen * 20.0f + 20.0f;
        
        float y = (logicalH - totalHeight) / 2.0f;
        
        if (!subtitulo.empty()) {
            box.AddText(L" === " + MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(subtitulo)) + L" ===", logicalW / 2.0f, y, 18.0f, D2D1::ColorF(0.6f, 0.6f, 0.9f), true);
            y += 30.0f;
        }
        
        for (int i = 0; i < maxLen; i++) {
            std::string line = "";
            if (i < arte.size()) line += arte[i];
            line += "  ";
            if (i < info.size()) line += info[i];
            box.AddText(MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(line)), logicalW / 2.0f, y, 16.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
            y += 20.0f;
        }

        y += 20.0f;
        MenuRaycasterUtils::adicionarOpcaoMenu(box, L"Voltar", logicalW / 2.0f, y, true, D2D1::ColorF(1.0f, 1.0f, 0.0f), true);
        
        box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.9f, 2.0f, D2D1::ColorF(1.0f, 0.6f, 0.0f), 20.0f, logicalW / 2.0f, logicalH / 2.0f);
        UIRenderer2D::ResetTransform(d2d);
    }
    ProcessarApresentarD2D();
}
