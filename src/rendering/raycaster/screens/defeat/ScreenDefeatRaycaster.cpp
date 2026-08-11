#include "ScreenDefeatRaycaster.h"
#include "../utils/MenuRaycasterUtils.h"
#include "../../engine-raycaster/Raycaster.h"
#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../ui/screens/defeat/ScreenDefeatLayout.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/window/GameWindow.h"
#include <thread>
#include <chrono>

namespace {
    using MenuRaycasterUtils::toWStringClean;
    using MenuRaycasterUtils::incrementarCicloDia;
}

void TelaDerrotaRaycaster::display(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido,
    int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns)
{
    InputControl::limparBuffer();

    int indexSelecionado = 0;
    bool popupAberto = false;
    bool saindoDoJogo = false;
    
    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        char tecla = InputControl::lerTecla();

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        incrementarCicloDia();

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            rt->BeginDraw();
            if (ID2D1Bitmap* tex = d2d->obterTexturaBackbuffer()) {
                D2D1_SIZE_U tsz = tex->GetPixelSize();
                D2D1_SIZE_F rsz = rt->GetSize();
                rt->DrawBitmap(tex, D2D1::RectF(0, 0, rsz.width, rsz.height), 0.35f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
                d2d->preencherRetangulo(0, 0, rsz.width, rsz.height, D2D1::ColorF(0.12f, 0.01f, 0.01f, 0.65f));
            }
        }

        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;
            float centerX = logicalW / 2.0f;

            UIDynamicBox mainBox;
            float startY = 80.0f;

            mainBox.AddText(L"V O C E   M O R R E U", centerX, startY, 28.0f, D2D1::ColorF(0.95f, 0.15f, 0.15f), true);
            startY += 45.0f;

            mainBox.AddText(L"--- ESTATÍSTICAS DA TENTATIVA ---", centerX, startY, 15.0f, D2D1::ColorF(1.0f, 0.4f, 0.4f), true);
            startY += 30.0f;

            std::string s1 = "Turnos Sobrevividos: " + std::to_string(combatTurns);
            mainBox.AddText(toWStringClean(s1), centerX, startY, 15.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), true);
            startY += 24.0f;

            std::string s2 = "Dano Causado: " + std::to_string(totalDamageDealt) + "   |   Dano Recebido: " + std::to_string(totalDamageTaken);
            mainBox.AddText(toWStringClean(s2), centerX, startY, 15.0f, D2D1::ColorF(0.9f, 0.6f, 0.6f), true);
            startY += 35.0f;

            // Interactive Options / Confirm Popup Centered inside/below Box
            if (popupAberto) {
                std::wstring msgConfirm = saindoDoJogo ? L"Tem certeza que deseja sair do jogo?" : L"Voltar ao Menu Principal?";
                mainBox.AddText(msgConfirm, centerX, startY, 16.0f, D2D1::ColorF(1.0f, 0.3f, 0.3f), true);
                startY += 30.0f;
                mainBox.AddText(L"[S] SIM      [N] NÃO", centerX, startY, 18.0f, D2D1::ColorF(1.0f, 0.85f, 0.0f), true);
            } else {
                std::wstring opt0 = (indexSelecionado == 0) ? L"> Voltar ao Menu Principal <" : L"  Voltar ao Menu Principal  ";
                std::wstring opt1 = (indexSelecionado == 1) ? L"> Sair do Jogo <" : L"  Sair do Jogo  ";

                D2D1_COLOR_F c0 = (indexSelecionado == 0) ? D2D1::ColorF(1.0f, 0.85f, 0.0f) : D2D1::ColorF(0.6f, 0.6f, 0.6f);
                D2D1_COLOR_F c1 = (indexSelecionado == 1) ? D2D1::ColorF(1.0f, 0.85f, 0.0f) : D2D1::ColorF(0.6f, 0.6f, 0.6f);

                mainBox.AddText(opt0, centerX, startY, 17.0f, c0, true);
                startY += 28.0f;
                mainBox.AddText(opt1, centerX, startY, 17.0f, c1, true);
            }

            mainBox.Render(d2d, D2D1::ColorF(0.06f, 0.01f, 0.01f), 0.94f, 2.5f, D2D1::ColorF(1.0f, 0.2f, 0.2f), 20.0f, centerX, 200.0f);

            UIRenderer2D::DrawTextNative(d2d, L"[ W / S ou Setas para navegar | ENTER para selecionar ]", centerX, logicalH - 50.0f, D2D1::ColorF(0.7f, 0.7f, 0.7f), 15.0f, true);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (popupAberto) {
            if (tecla == 's' || tecla == 'S') {
                if (saindoDoJogo) {
                    exit(0);
                } else {
                    return; 
                }
            } else if (tecla == 'n' || tecla == 'N') {
                popupAberto = false;
            }
        } else {
            if (tecla == 'w' || tecla == 'W') { 
                indexSelecionado--;
                if (indexSelecionado < 0) indexSelecionado = 1;
            } else if (tecla == 's' || tecla == 'S') { 
                indexSelecionado++;
                if (indexSelecionado > 1) indexSelecionado = 0;
            } else if (tecla == '\r' || tecla == '\n' || tecla == ' ') {
                saindoDoJogo = (indexSelecionado == 1);
                popupAberto = true;
            }
        }
    }
}

