#include "ScreenVictoryRaycaster.h"
#include "../utils/MenuRaycasterUtils.h"
#include "../../engine-raycaster/Raycaster.h"
#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../ui/screens/victory/ScreenVictoryLayout.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/window/GameWindow.h"
#include <thread>
#include <chrono>
#include <map>

namespace {
    using MenuRaycasterUtils::toWStringClean;
}

void TelaVitoriaRaycaster::display(Character* currentPlayer, int goldEarned, int xpEarned,
    int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns,
    const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano,
    int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::pair<std::string, int>>& dropsUnicos,
    bool podeSubirNivel, const std::vector<std::string>& novasDescobertas,
    const std::string& tituloMapa)
{
    InputControl::limparBuffer();
    std::map<std::string, int> inimigosAgrupados;
    for (const auto& ini : inimigosDerrotados) {
        inimigosAgrupados[ini]++;
    }

    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        char tecla = InputControl::lerTecla();

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        d2d->obterRenderTarget()->BeginDraw();

        // 1. Renderiza impressao 3D do mapa de fundo do combate (mesmo do combate)
        if (ID2D1Bitmap* tex = d2d->obterTexturaBackbuffer()) {
            D2D1_SIZE_U tsz = tex->GetPixelSize();
            D2D1_SIZE_F rsz = d2d->obterRenderTarget()->GetSize();
            d2d->obterRenderTarget()->DrawBitmap(tex, D2D1::RectF(0, 0, rsz.width, rsz.height), 0.45f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
            d2d->preencherRetangulo(0, 0, rsz.width, rsz.height, D2D1::ColorF(0.02f, 0.02f, 0.05f, 0.45f));
        } else {
            MenuRaycasterUtils::desenharFundoNativoD2D(true);
        }

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            // Caixa centralizada de vitoria (titulo, estatisticas de combate e itens)
            UIDynamicBox vicBox;
            float centerX = logicalW / 2.0f;
            float startY = 40.0f;

            vicBox.AddText(L"V I T O R I A", centerX, startY, 26.0f, D2D1::ColorF(1.0f, 0.85f, 0.0f), true);
            startY += 40.0f;

            if (currentPlayer) {
                std::string playerSub = currentPlayer->getName() + " (Nível " + std::to_string(currentPlayer->getLevel()) + ")";
                vicBox.AddText(toWStringClean(playerSub), centerX, startY, 15.0f, D2D1::ColorF(0.80f, 0.85f, 0.90f), true);
                startY += 30.0f;
            }

            // Estatisticas do Combate
            vicBox.AddText(L"--- ESTATÍSTICAS DO COMBATE ---", centerX, startY, 15.0f, D2D1::ColorF(0.6f, 0.8f, 1.0f), true);
            startY += 28.0f;

            std::string line1 = "Turnos: " + std::to_string(combatTurns) + "  |  Dano Causado: " + std::to_string(totalDamageDealt) + "  |  Dano Recebido: " + std::to_string(totalDamageTaken);
            vicBox.AddText(toWStringClean(line1), centerX, startY, 14.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), true);
            startY += 24.0f;

            std::string line2 = "Cura Recebida: " + std::to_string(totalHealingReceived) + "  |  Maior Hit: " + std::to_string(maiorDano) + "  |  Parries Perfeitos: " + std::to_string(parriesPerfeitos);
            vicBox.AddText(toWStringClean(line2), centerX, startY, 14.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), true);
            startY += 32.0f;

            // Inimigos Derrotados
            if (!inimigosAgrupados.empty()) {
                std::string iniListStr = "Derrotados: ";
                size_t count = 0;
                for (auto const& [nome, qtd] : inimigosAgrupados) {
                    iniListStr += std::to_string(qtd) + "x " + nome + (++count < inimigosAgrupados.size() ? ", " : "");
                }
                vicBox.AddText(toWStringClean(iniListStr), centerX, startY, 14.0f, D2D1::ColorF(1.0f, 0.4f, 0.4f), true);
                startY += 32.0f;
            }

            // Secao de Recompensas & Drops abaixo
            vicBox.AddText(L"--- RECOMPENSAS & DROPS ---", centerX, startY, 15.0f, D2D1::ColorF(1.0f, 0.85f, 0.0f), true);
            startY += 28.0f;

            std::string rewStr = "+" + std::to_string(xpEarned) + " XP   |   +" + std::to_string(goldEarned) + " Ouro";
            vicBox.AddText(toWStringClean(rewStr), centerX, startY, 16.0f, D2D1::ColorF(0.2f, 1.0f, 0.4f), true);
            startY += 26.0f;

            if (!dropsUnicos.empty()) {
                for (auto const& drop : dropsUnicos) {
                    std::string itemLine = "- " + std::to_string(drop.second) + "x " + drop.first;
                    vicBox.AddText(toWStringClean(itemLine), centerX, startY, 14.0f, D2D1::ColorF(1.0f, 0.90f, 0.50f), true);
                    startY += 22.0f;
                }
            }

            vicBox.Render(d2d, D2D1::ColorF(0.04f, 0.04f, 0.07f), 0.92f, 2.5f, D2D1::ColorF(1.0f, 0.85f, 0.0f), 20.0f, centerX, 240.0f);

            UIRenderer2D::DrawTextNative(d2d, L"[ Pressione ENTER ou ESPAÇO para continuar ]", centerX, logicalH - 50.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f), 17.0f, true);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (tecla == '\r' || tecla == '\n' || tecla == ' ') {
            break;
        }
    }
}

