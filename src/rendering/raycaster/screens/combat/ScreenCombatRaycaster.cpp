#include "ScreenCombatRaycaster.h"
#include "../../engine-raycaster/Raycaster.h"
#include "../../../../ui/screens/combat/ScreenCombat.h"
#include "../../../../systems/combat/Combat.h"
#include "../../../../systems/combat/Parry.h"
#include "../../../../entities/character/Character.h"
#include "../../../../entities/races/RaceBase.h"
#include "../../../../systems/inventory/Item.h"
#include "../../../../systems/inventory/Inventory.h"
#include "../utils/MenuRaycasterUtils.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../core/window/GameWindow.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/utils/InputControl.h"
#include "../../engine-raycaster/RaycasterRendererCombat.h"
#include "../../engine-raycaster/RaycasterHUD.h"
#include "../../engine-raycaster/RaycasterRenderer.h"
#include "../../engine-raycaster/RaycasterWorld.h"
#include "../../engine-raycaster/RaycasterEnemies.h"
#include "../../engine-raycaster/RaycasterNPCs.h"
#include "../../../../rendering/direct-2d/RaycasterD2DUtils.h"

#include <chrono>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <iostream>


// State Estatico de Combate

static bool s_isContexto3D = false;
static std::vector<std::string> s_contextoMapa;
static float s_contextoPosX = 0.0f;
static float s_contextoPosY = 0.0f;
static float s_contextoAngulo = 0.0f;
static std::string s_contextoTituloMapa = "";
static std::string s_tituloTurnoHUD = "";
static std::string s_mensagemBannerCombate = "";
static CorBanner s_corBannerCombate = CorBanner::OURO;
static std::string s_mensagemBannerLinha2 = "";
static bool s_backdropMapDirty = true;


static std::vector<std::string> s_historicoLogCompleto;
static bool s_logExpandido = false;

struct PlayerHUDStateRaycaster {
    double hpFantasma = -1.0;
    double hpAnterior = -1.0;
};
static std::unordered_map<Character*, PlayerHUDStateRaycaster> hudStatesRaycaster;

// Estrutura de particulas de poeira Direct2D para animacao de morte do inimigo
struct DustParticle {
    float x, y;
    float vx, vy;
    float alpha;
    float size;
};

void TelaCombateRaycaster::adicionarMensagemFixa(const std::string& msg) {
    if (msg.empty()) return;
    std::string clean = MenuRaycasterUtils::stripAnsi(msg);
    s_historicoLogCompleto.push_back(clean);
    if (s_historicoLogCompleto.size() > 100) {
        s_historicoLogCompleto.erase(s_historicoLogCompleto.begin());
    }
}

void TelaCombateRaycaster::limparMensagensFixas() {
    // Mantem o historico de registro intacto para rastreamento completo do combate
}

void TelaCombateRaycaster::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    s_isContexto3D = modo3D;
    s_contextoMapa = matriz;
    s_contextoPosX = posX;
    s_contextoPosY = posY;
    s_contextoAngulo = angulo;
    s_contextoTituloMapa = titulo;
    s_backdropMapDirty = true;
}

void TelaCombateRaycaster::definirTurnoVisivel(int turno, const std::string& nome) {
    (void)turno;
    s_tituloTurnoHUD = "[ TURNO DE " + nome + " ]";
}

void TelaCombateRaycaster::definirMensagemBanner(const std::string& msg, CorBanner cor, const std::string& msgLinha2) {
    s_mensagemBannerCombate = msg;
    s_corBannerCombate = cor;
    s_mensagemBannerLinha2 = msgLinha2;

    if (!msg.empty()) {
        adicionarMensagemFixa(msg);
    }
    if (!msgLinha2.empty()) {
        adicionarMensagemFixa(msgLinha2);
    }
}

// Renderizador principal de quadro de combate Direct2D (fundo 3D e inimigos em Direct2D)
static void renderizarQuadroCombateD2D(
    Character* currentPlayer,
    const std::vector<Character*>& enemies,
    int opcaoMenuSelecionada = -1,
    int alvoSelecionado = -1,
    Character* alvoAnimacao = nullptr,
    int frameAnimacao = 0,
    int danoAmount = -1,
    bool isCura = false,
    bool isMorte = false,
    int flashTipoInimigo = 0, // 0: Normal, 1: Dano (Vermelho), 2: Ataque (Laranja), 3: Cura (Verde), 4: Selecionado (Amarelo)
    std::vector<DustParticle>* dustParticles = nullptr
) {
    auto d2d = D2DContext::renderer;
    if (!d2d) return;

    auto agora = std::chrono::steady_clock::now();
    static auto tempoInicio = agora;
    float tempoAbsoluto = std::chrono::duration<float>(agora - tempoInicio).count();
    int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());

    // 1. Renderiza visao 3D do mapa sem inimigos de fundo e com escurecimento (cache para 60 FPS)
    if (s_isContexto3D && !s_contextoMapa.empty()) {
        if (s_backdropMapDirty) {
            int bbW = 640;
            int bbH = 360;
            std::vector<Pixel3D> tela3D(bbW * bbH);
            
            // Remove entidades inimigas do mapa para que a renderizacao 3D de fundo fique limpa
            std::vector<std::string> mapaSemInimigos = s_contextoMapa;
            std::string inimigosChars = "GOBFPMSTRCH";
            for (auto& linha : mapaSemInimigos) {
                for (char& c : linha) {
                    if (inimigosChars.find(c) != std::string::npos) {
                        c = '.'; // Substitui a celula do inimigo no mapa por chao
                    }
                }
            }

            bool temaFloresta = RaycasterWorld::isTemaFloresta(s_contextoTituloMapa);
            int temaCeu = RaycasterWorld::obterTemaCeu(s_contextoTituloMapa);
            int temaAtivo = temaCeu;

            const auto& cacheSprites = RaycasterNPCs::obterCacheGlobal();

            RaycasterRenderer::renderizar3D(
                tela3D, bbW, bbH, s_contextoPosX, s_contextoPosY, s_contextoAngulo, 
                bbH / 2.0f, 0, 150.0f, tempoAbsoluto, mapaSemInimigos, s_contextoTituloMapa, 
                temaFloresta, temaAtivo, cacheSprites
            );


            RaycasterD2DUtils::preencherBackbuffer(*d2d, tela3D, bbW, bbH);
            d2d->copiarBackbufferParaTextura();
            s_backdropMapDirty = false;
        }

        d2d->obterRenderTarget()->BeginDraw();
        if (ID2D1Bitmap* tex = d2d->obterTexturaBackbuffer()) {
            D2D1_SIZE_U tsz = tex->GetPixelSize();
            D2D1_SIZE_F rsz = d2d->obterRenderTarget()->GetSize();
            // Desenha backbuffer do mapa escurecido (opacidade 0.50f)
            d2d->obterRenderTarget()->DrawBitmap(tex, D2D1::RectF(0, 0, rsz.width, rsz.height), 0.50f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
            // Adiciona sobreposicao escura sutil para destacar a visibilidade do sprite inimigo
            d2d->preencherRetangulo(0, 0, rsz.width, rsz.height, D2D1::ColorF(0.02f, 0.02f, 0.05f, 0.35f));
        }
    } else {
        MenuRaycasterUtils::desenharFundoNativoD2D(true);
        auto rtBackground = d2d->obterRenderTarget();
        if (rtBackground) {
            D2D1_SIZE_F rsz = rtBackground->GetSize();
            d2d->preencherRetangulo(0, 0, rsz.width, rsz.height, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.40f));
        }
    }


    auto rt = d2d->obterRenderTarget();
    if (!rt) return;



    D2D1_SIZE_F tam = rt->GetSize();
    UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

    float logicalW = UIRenderer2D::LOGICAL_WIDTH;
    float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

    // 2. Renderiza inimigos alinhados horizontalmente com balanco e escala correta
    int totalEnemies = (int)enemies.size();
    if (totalEnemies > 0) {
        float areaWidth = std::min(1300.0f, logicalW * 0.70f);
        float startX = (logicalW - areaWidth) / 2.0f + (areaWidth / (totalEnemies + 1));
        float stepX = (totalEnemies > 1) ? (areaWidth / (totalEnemies + 1)) : 0.0f;
        if (totalEnemies == 1) startX = logicalW / 2.0f;

        for (int i = 0; i < totalEnemies; ++i) {
            Character* enemy = enemies[i];
            if (!enemy) continue;
            if (enemy->obterMorteAnimada() || (enemy->obterVida() <= 0 && enemy != alvoAnimacao)) {
                continue; // Skip dead enemies, keeping current animation target seamlessly visible
            }

            // Sway motion (side-to-side sine wave)
            float swayX = std::sin(tempoAbsoluto * 2.5f + i * 1.4f) * 12.0f;
            float enemyX = (totalEnemies == 1) ? (startX + swayX) : (startX + i * stepX + swayX);
            float enemyY = 420.0f;

            auto arte = enemy->obterRaca() ? enemy->obterRaca()->getRaceAppearance() : std::vector<std::string>{"[ ? ]"};
            auto corBase = RaycasterRendererCombate::obterCorSpriteInimigo(enemy);

            // Escala proporcional do sprite do inimigo
            float pixelScale = (totalEnemies > 2) ? 3.5f : 4.0f;
            float spriteWidth = (arte.empty() ? 40.0f : arte[0].size() * pixelScale);
            float spriteHeight = arte.size() * pixelScale;

            // Determinar tonalidade e efeitos de brilho
            D2D1_COLOR_F corFinal = D2D1::ColorF(std::get<0>(corBase) / 255.0f, std::get<1>(corBase) / 255.0f, std::get<2>(corBase) / 255.0f);

            bool piscarAmareloAlvo = (alvoSelecionado == i) && ((tempoMs / 100) % 2 == 0);
            bool piscarVermelhoDano = (enemy == alvoAnimacao && flashTipoInimigo == 1) && ((frameAnimacao % 2 == 0) || ((tempoMs / 40) % 2 == 0));

            if (enemy == Parry::obterInimigoAtacante() && flashTipoInimigo != 1) {
                corFinal = D2D1::ColorF(1.0f, 0.50f, 0.0f); // Tonalidade Laranja para Inimigo Atacante
            }

            if (enemy == alvoAnimacao) {
                if (flashTipoInimigo == 1) {
                    corFinal = piscarVermelhoDano ? D2D1::ColorF(1.0f, 0.15f, 0.15f) : D2D1::ColorF(1.0f, 0.90f, 0.90f); // Piscar Vermelho/Branco de Dano
                } else if (flashTipoInimigo == 2) {
                    corFinal = D2D1::ColorF(1.0f, 0.5f, 0.0f); // Laranja Ataque
                } else if (flashTipoInimigo == 3) {
                    corFinal = D2D1::ColorF(0.15f, 1.0f, 0.15f); // Verde Cura
                }
            }

            if (piscarAmareloAlvo) {
                corFinal = D2D1::ColorF(1.0f, 1.0f, 0.0f); // Amarelo Piscar Alvo Selecionado
            }

            std::vector<GrupoCorUI> paletaD2D = {
                {"#@O0o*+.X=", (int)(corFinal.r * 255), (int)(corFinal.g * 255), (int)(corFinal.b * 255)}
            };

            // Animacao de particulas de morte ou desenho padrao (virar poeira)
            if (isMorte && enemy == alvoAnimacao) {
                float opacity = std::max(0.0f, 1.0f - (frameAnimacao / 20.0f));
                UIRenderer2D::DrawPixelArt(d2d, arte, paletaD2D, enemyX - spriteWidth / 2.0f, enemyY - spriteHeight / 2.0f, pixelScale, opacity);
                
                if (dustParticles) {
                    for (auto& p : *dustParticles) {
                        float fadeR = 0.90f;
                        float fadeG = 0.80f - (1.0f - opacity) * 0.35f;
                        float fadeB = 0.60f - (1.0f - opacity) * 0.35f;
                        d2d->preencherRetangulo(enemyX + p.x, enemyY + p.y, p.size, p.size, D2D1::ColorF(fadeR, fadeG, fadeB, p.alpha * opacity));
                    }
                }
            } else {
                UIRenderer2D::DrawPixelArt(d2d, arte, paletaD2D, enemyX - spriteWidth / 2.0f, enemyY - spriteHeight / 2.0f, pixelScale, 1.0f);
            }

            // Nome do inimigo, icones de status e barra de vida sobre o sprite
            UIDynamicBox enemyBox;
            float topY = enemyY - (spriteHeight / 2.0f) - 60.0f;

            // Renderiza emblemas de status do inimigo (apenas texto, sem emojis)
            std::vector<std::string> statusBadges;
            if (enemy->obterDefendendo()) statusBadges.push_back("[DEFESA]");
            if (enemy->possuiEfeito(EfeitoID::Atordoamento)) statusBadges.push_back("[ATORDOADO]");
            if (enemy->possuiEfeito(EfeitoID::Sangramento)) statusBadges.push_back("[SANGRAMENTO]");
            if (enemy->possuiEfeito(EfeitoID::QuebraResistencia)) statusBadges.push_back("[PO MAGICO]");
            if (enemy->possuiEfeito(EfeitoID::RodaAdaptacao)) statusBadges.push_back("[ADAPTACAO]");
            if (enemy->possuiEfeito(EfeitoID::Inviolavel)) statusBadges.push_back("[INVIOLAVEL]");
            if (enemy->possuiEfeito(EfeitoID::Necrose)) statusBadges.push_back("[NECROSE]");

            if (!statusBadges.empty()) {
                std::string badgeStr = "";
                for (size_t b = 0; b < statusBadges.size(); ++b) {
                    if (b > 0) badgeStr += " ";
                    badgeStr += statusBadges[b];
                }
                float badgeY = (alvoSelecionado == i) ? (topY - 42.0f) : (topY - 22.0f);
                enemyBox.AddText(MenuRaycasterUtils::utf8_to_wstring(badgeStr), enemyX, badgeY, 13.0f, D2D1::ColorF(0.3f, 0.9f, 1.0f), true);
            }

            if (alvoSelecionado == i) {
                enemyBox.AddText(L"[ ALVO SELECIONADO ]", enemyX, topY - 22.0f, 15.0f, D2D1::ColorF(1.0f, 1.0f, 0.0f), true);
            }

            std::string nomeInimigo = enemy->getName() + " (" + std::to_string(i + 1) + ")";
            enemyBox.AddText(MenuRaycasterUtils::utf8_to_wstring(nomeInimigo), enemyX, topY, 17.0f, piscarAmareloAlvo ? D2D1::ColorF(1.0f, 1.0f, 0.0f) : D2D1::ColorF(1.0f, 1.0f, 1.0f), true);


            // HP Bar
            double pctHp = enemy->obterVida() / (double)std::max(1, enemy->obterVidaMaxima());
            std::string hpStr = std::to_string(enemy->obterVida()) + " / " + std::to_string(enemy->obterVidaMaxima()) + " HP";
            D2D1_COLOR_F corHpBar = (pctHp > 0.6) ? D2D1::ColorF(0.2f, 0.9f, 0.2f) : (pctHp > 0.25) ? D2D1::ColorF(0.9f, 0.9f, 0.2f) : D2D1::ColorF(0.9f, 0.2f, 0.2f);
            
            float barW = 150.0f;
            float barH = 14.0f;
            float barX = enemyX - (barW / 2.0f);
            float barY = topY + 24.0f;

            enemyBox.AddRect(barX, barY, barW, barH, D2D1::ColorF(0.2f, 0.2f, 0.2f, 0.8f));
            enemyBox.AddRect(barX, barY, barW * (float)pctHp, barH, corHpBar);
            enemyBox.AddText(MenuRaycasterUtils::utf8_to_wstring(hpStr), enemyX, barY - 1.0f, 12.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);

            // Floating Damage Number overlay
            if (enemy == alvoAnimacao && danoAmount >= 0) {
                std::string msgDano = (isCura ? "+" : "-") + std::to_string(danoAmount);
                D2D1_COLOR_F corDanoNum = isCura ? D2D1::ColorF(0.2f, 1.0f, 0.2f) : D2D1::ColorF(1.0f, 0.2f, 0.2f);
                enemyBox.AddText(MenuRaycasterUtils::utf8_to_wstring(msgDano), enemyX, topY - 45.0f, 22.0f, corDanoNum, true);
            }

            enemyBox.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f), 0.85f, 1.5f, piscarAmareloAlvo ? D2D1::ColorF(1.0f, 1.0f, 0.0f) : D2D1::ColorF(0.4f, 0.4f, 0.5f), 10.0f, enemyX, topY + 12.0f);
        }
    }

    // 3. Renderiza HUD de exploracao do jogador na base
    if (currentPlayer) {
        RaycasterHUD::desenharProcedural(*d2d, (int)tam.width, (int)tam.height, s_contextoPosX, s_contextoPosY, s_contextoAngulo, s_contextoMapa, s_contextoTituloMapa, false, currentPlayer);
    }

    // 4. Renderiza barra de opcoes de combate ou aviso acima do HUD do jogador
    RaycasterHUD::desenharOpcoesCombateProcedural(*d2d, (int)tam.width, (int)tam.height, opcaoMenuSelecionada, s_mensagemBannerCombate, s_corBannerCombate, s_mensagemBannerLinha2);

    // 4b. Renderiza sobreposicao do minigame de aparo no Direct2D se ativo
    Parry::renderizarOverlaysD2D(*d2d, (int)tam.width, (int)tam.height);

    // 5. Painel expansivel de registro de combate em tempo real (lado direito)
    float logX = logicalW - 440.0f;
    float logY = 50.0f;

    UIDynamicBox logPanelBox;
    if (s_logExpandido) {
        logPanelBox.AddText(L"LOG DE BATALHA (Clique/L para recolher)", logX + 200.0f, logY, 15.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);

        // Barra de ordem dos turnos no topo do registro expandido
        std::vector<Character*> ordemTurnos;
        if (currentPlayer) ordemTurnos.push_back(currentPlayer);
        for (Character* enemy : enemies) {
            if (enemy && enemy->obterVida() > 0) ordemTurnos.push_back(enemy);
        }
        std::sort(ordemTurnos.begin(), ordemTurnos.end(), [](Character* a, Character* b) {
            return a->getDexterity() > b->getDexterity();
        });

        std::string textoOrdem = "ORDEM: ";
        for (size_t t = 0; t < ordemTurnos.size(); ++t) {
            if (t > 0) textoOrdem += " -> ";
            std::string nomeChar = ordemTurnos[t]->getName();
            if (ordemTurnos[t] == currentPlayer) nomeChar = "[" + nomeChar + "]";
            else if (ordemTurnos[t] == Parry::obterInimigoAtacante()) nomeChar = "*" + nomeChar + "*";
            textoOrdem += nomeChar;
        }

        logPanelBox.AddText(MenuRaycasterUtils::utf8_to_wstring(textoOrdem), logX + 200.0f, logY + 22.0f, 13.0f, D2D1::ColorF(0.3f, 0.9f, 1.0f), true);
        float currY = logY + 48.0f;
        
        int startIdx = std::max(0, (int)s_historicoLogCompleto.size() - 20);
        for (int i = startIdx; i < (int)s_historicoLogCompleto.size(); ++i) {
            std::string msg = s_historicoLogCompleto[i];
            D2D1_COLOR_F corMsg = D2D1::ColorF(0.85f, 0.85f, 0.85f);
            if (msg.find("dano") != std::string::npos || msg.find("perdeu") != std::string::npos || msg.find("damage") != std::string::npos || msg.find("causou") != std::string::npos) corMsg = D2D1::ColorF(1.0f, 0.4f, 0.4f);
            else if (msg.find("curou") != std::string::npos || msg.find("HP") != std::string::npos || msg.find("recuperou") != std::string::npos) corMsg = D2D1::ColorF(0.4f, 1.0f, 0.4f);
            else if (msg.find("Parry") != std::string::npos || msg.find("TURNO") != std::string::npos || msg.find("Grito") != std::string::npos) corMsg = D2D1::ColorF(1.0f, 0.84f, 0.0f);

            logPanelBox.AddText(MenuRaycasterUtils::utf8_to_wstring(msg), logX + 10.0f, currY, 13.0f, corMsg, false);
            currY += 22.0f;
        }
        logPanelBox.Render(d2d, D2D1::ColorF(0.02f, 0.02f, 0.05f), 0.94f, 2.0f, D2D1::ColorF(0.5f, 0.5f, 0.7f), 15.0f, logX + 200.0f, logY + 220.0f);
    } else {
        logPanelBox.AddText(L"LOG DE BATALHA [Clique / L]", logX + 200.0f, logY, 14.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f), true);

        if (!s_historicoLogCompleto.empty()) {
            std::string ultimaMsg = s_historicoLogCompleto.back();
            logPanelBox.AddText(MenuRaycasterUtils::utf8_to_wstring(ultimaMsg), logX + 200.0f, logY + 24.0f, 12.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);
        }
        logPanelBox.Render(d2d, D2D1::ColorF(0.02f, 0.02f, 0.05f), 0.85f, 1.5f, D2D1::ColorF(0.5f, 0.5f, 0.5f), 10.0f, logX + 200.0f, logY + 12.0f);
    }

    // 6. Direct2D Parry QTE Popup Box (Movement / Typing Sequence - Tutorial UI Appearance)
    std::string parryMsg = Parry::minigameMessage;
    std::string parryBar = Parry::minigameBar;
    if (!parryMsg.empty() || !parryBar.empty() || (Parry::barSize > 0 && Parry::cursorPos >= 0)) {
        UIDynamicBox parryBox;
        float pX = logicalW / 2.0f;
        float pY = logicalH / 2.0f - 30.0f;

        bool isFlash = (parryMsg.find("FLASH") != std::string::npos || parryMsg.find("PREPARE-SE") != std::string::npos);
        D2D1_COLOR_F borderColor = isFlash ? D2D1::ColorF(1.0f, 0.95f, 0.2f) : D2D1::ColorF(1.0f, 0.85f, 0.0f);
        D2D1_COLOR_F bgColor = isFlash ? D2D1::ColorF(0.35f, 0.05f, 0.05f) : D2D1::ColorF(0.04f, 0.02f, 0.02f);
        D2D1_COLOR_F textColor = isFlash ? D2D1::ColorF(1.0f, 1.0f, 0.3f) : D2D1::ColorF(1.0f, 1.0f, 1.0f);

        parryBox.AddText(L"--- REAÇÃO DE PARRY (QTE) ---", pX, pY - 45.0f, 18.0f, D2D1::ColorF(1.0f, 0.85f, 0.0f), true);

        if (!parryMsg.empty()) {
            std::string cleanMsg = MenuRaycasterUtils::stripAnsi(parryMsg);
            parryBox.AddText(MenuRaycasterUtils::utf8_to_wstring(cleanMsg), pX, pY - 15.0f, 16.0f, textColor, true);
        }

        // Graphical Tutorial Parry Bar
        if (Parry::barSize > 0 && Parry::cursorPos >= 0) {
            float barW = 320.0f;
            float barH = 26.0f;
            float barLeft = pX - barW / 2.0f;
            float barTop = pY + 20.0f;
            float unitW = barW / (float)Parry::barSize;

            parryBox.AddRect(barLeft, barTop, barW, barH, D2D1::ColorF(0.18f, 0.18f, 0.20f, 0.95f));
            float ssLeft = barLeft + (Parry::sweetSpotCenter - Parry::sweetSpotSize / 2.0f) * unitW;
            float ssW = Parry::sweetSpotSize * unitW;
            parryBox.AddRect(ssLeft, barTop, ssW, barH, D2D1::ColorF(1.0f, 0.85f, 0.0f, 1.0f)); // Bordas amarelas

            float centerLeft = barLeft + (Parry::sweetSpotCenter - Parry::sweetSpotSize / 4.0f) * unitW;
            float centerW = (Parry::sweetSpotSize / 2.0f) * unitW;
            parryBox.AddRect(centerLeft, barTop, centerW, barH, D2D1::ColorF(0.0f, 0.75f, 0.2f, 1.0f)); // Centro verde

            float curLeft = barLeft + Parry::cursorPos * unitW;
            parryBox.AddRect(curLeft, barTop, std::max(unitW, 6.0f), barH, D2D1::ColorF(0.0f, 0.85f, 1.0f, 1.0f));
        } else if (!parryBar.empty()) {
            std::string cleanBar = MenuRaycasterUtils::stripAnsi(parryBar);
            parryBox.AddText(MenuRaycasterUtils::utf8_to_wstring(cleanBar), pX, pY + 20.0f, 16.0f, D2D1::ColorF(0.4f, 1.0f, 0.4f), true);
        }

        parryBox.Render(d2d, bgColor, 0.95f, isFlash ? 5.0f : 3.0f, borderColor, 20.0f, pX, pY);
    }


    UIRenderer2D::ResetTransform(d2d);
    rt->EndDraw();
}

// Implementacao dos metodos da classe TelaCombateRaycaster

void TelaCombateRaycaster::displayLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) {
    (void)tituloDaTela; (void)animar;
}

void TelaCombateRaycaster::animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) {
    (void)titulo; (void)enemies; (void)currentPlayer;
}

std::vector<std::string> TelaCombateRaycaster::obterLinhasBarraDeStatusDoJogador(Character* currentPlayer, Color corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) {
    (void)corDestaque; (void)danoAnimacao; (void)frameAnimacao; (void)isCura;
    if (!currentPlayer) return {};
    return { currentPlayer->getName(), "HP: " + std::to_string(currentPlayer->obterVida()) };
}

void TelaCombateRaycaster::displayHordaDeInimigosLadoALado(const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) {
    (void)animarSurgimento; (void)armaAtacante; (void)dropsAnimacao;
    renderizarQuadroCombateD2D(nullptr, listaDeInimigos, -1, -1, alvoAnimacao, frameAnimacao, danoAnimacao, isCura, isMorte, 0);
}

void TelaCombateRaycaster::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada, std::function<void()> callbackD2DOverlay) {
    (void)tituloCombate; (void)listaDeAliados; (void)animarEntrada; (void)callbackD2DOverlay;
    renderizarQuadroCombateD2D(currentPlayer, listaDeInimigos, -1, -1, nullptr, 0, -1, false, false, 0);
}

void TelaCombateRaycaster::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) {
    (void)tituloCombate; (void)atacante; (void)listaDeAliados;
    InputControl::limparBuffer();
    
    int frameCounter = 1;
    auto inicio = std::chrono::steady_clock::now();

    while (true) {
        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::atualizarTeclas();

        char tecla = InputControl::lerTecla();
        auto decorridoMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - inicio).count();

        // O inimigo fica piscando em vermelho ate o jogador pressionar ENTER
        renderizarQuadroCombateD2D(currentPlayer, listaDeInimigos, -1, -1, alvoAnimacao, ((frameCounter % 14) + 1), danoAnimacao, false, false, 1);

        if (decorridoMs > 150 && (tecla == '\r' || tecla == '\n' || tecla == ' ')) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        frameCounter++;
    }
    InputControl::limparBuffer();
}

void TelaCombateRaycaster::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    (void)tituloCombate; (void)listaDeAliados;
    if (alvoAnimacao && curaAnimacao > 0) {
        adicionarMensagemFixa(alvoAnimacao->getName() + " curou " + std::to_string(curaAnimacao) + " HP!");
    }
    for (int frame = 1; frame <= 8; ++frame) {
        renderizarQuadroCombateD2D(currentPlayer, listaDeInimigos, -1, -1, alvoAnimacao, frame, curaAnimacao, true, false, 3);
        if (D2DContext::window) D2DContext::window->processarMensagens();
        InputControl::atualizarTeclas();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void TelaCombateRaycaster::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) {
    (void)tituloCombate; (void)alvoAnimacao; (void)listaDeAliados; (void)isParry;
    InputControl::limparBuffer();
    RaycasterHUD::gatilharShakeVida();

    int frameCounter = 1;
    auto inicio = std::chrono::steady_clock::now();

    while (true) {
        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::atualizarTeclas();

        char tecla = InputControl::lerTecla();
        auto decorridoMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - inicio).count();

        // O jogador e a tela piscam em vermelho/laranja ate o jogador pressionar ENTER
        renderizarQuadroCombateD2D(currentPlayer, listaDeInimigos, -1, -1, nullptr, ((frameCounter % 6) + 1), danoAnimacao, false, false, 0);

        if (decorridoMs > 150 && (tecla == '\r' || tecla == '\n' || tecla == ' ')) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        frameCounter++;
    }
    InputControl::limparBuffer();
}


void TelaCombateRaycaster::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    (void)tituloCombate; (void)alvoAnimacao; (void)listaDeAliados;
    for (int frame = 1; frame <= 4; ++frame) {
        renderizarQuadroCombateD2D(currentPlayer, listaDeInimigos, -1, -1, nullptr, frame, curaAnimacao, true, false, 0);
        if (D2DContext::window) D2DContext::window->processarMensagens();
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
}


void TelaCombateRaycaster::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) {
    (void)tituloCombate; (void)listaDeAliados; (void)drops;
    
    // Gerar 120 particulas de poeira e brasas para a animacao de dissolucao do inimigo
    std::vector<DustParticle> particles;
    for (int i = 0; i < 120; ++i) {
        DustParticle p;
        p.x = (rand() % 110) - 55.0f;
        p.y = (rand() % 95) - 47.0f;
        p.vx = ((rand() % 100) - 50.0f) / 10.0f;
        p.vy = -((rand() % 100) / 8.0f) - 1.2f;
        p.alpha = 1.0f;
        p.size = (rand() % 6) + 2.0f;
        particles.push_back(p);
    }

    for (int frame = 1; frame <= 24; ++frame) {
        for (auto& p : particles) {
            p.x += p.vx + std::sin(frame * 0.35f + p.y * 0.1f) * 0.9f;
            p.y += p.vy;
            p.alpha = std::max(0.0f, p.alpha - 0.040f);
        }
        renderizarQuadroCombateD2D(currentPlayer, listaDeInimigos, -1, -1, inimigoMorto, frame, -1, false, true, 0, &particles);
        if (D2DContext::window) D2DContext::window->processarMensagens();
        InputControl::atualizarTeclas();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}


// Entrada do menu de acao do jogador

int TelaCombateRaycaster::obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    (void)turnoAtual; (void)personagemAgindo; (void)aliados;
    InputControl::limparBuffer();
    GameWindow::limparMouse();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    InputControl::limparBuffer();

    // Notificacao exigindo ENTER do jogador para abrir as opcoes de acao
    definirMensagemBanner("TURNO DO JOGADOR - PRESSIONE ENTER PARA ESCOLHER UMA ACAO", CorBanner::OURO);
    renderizarQuadroCombateD2D(currentPlayer, enemies, -1, -1, nullptr, 0, -1, false, false, 0);
    InputControl::aguardarEnter("Pressione ENTER para escolher uma acao...");
    InputControl::limparBuffer();

    int selecionado = 0;
    std::vector<std::string> acoes = {"Atacar", "Habilidade", "Defender", "Itens", "Diário"};

    while (true) {
        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::atualizarTeclas();

        char tecla = InputControl::lerTecla();

        // Alternancia do painel de registro via clique do mouse
        if (GameWindow::mouseClicado()) {
            int mx = GameWindow::obterMouseX();
            int my = GameWindow::obterMouseY();
            GameWindow::limparMouse();

            float logX = UIRenderer2D::LOGICAL_WIDTH - 440.0f;
            if (mx >= logX) {
                s_logExpandido = !s_logExpandido;
            }
        }

        if (tecla == 'l' || tecla == 'L') {
            s_logExpandido = !s_logExpandido;
        }

        renderizarQuadroCombateD2D(currentPlayer, enemies, selecionado, -1, nullptr, 0, -1, false, false, 0);

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (tecla == 'a' || tecla == 'A' || tecla == 75) {
            selecionado = (selecionado - 1 + (int)acoes.size()) % (int)acoes.size();
        } else if (tecla == 'd' || tecla == 'D' || tecla == 77) {
            selecionado = (selecionado + 1) % (int)acoes.size();
        } else if (tecla == '\r' || tecla == '\n' || tecla == ' ') {
            if (selecionado == 0) return 1; // Atacar
            if (selecionado == 1) return 3; // Habilidade
            if (selecionado == 2) return 2; // Defender
            if (selecionado == 3) return 4; // Itens
            if (selecionado == 4) return 6; // Diario
        }
    }
}


// Selecao manual de alvo com teclas A e D


int TelaCombateRaycaster::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    (void)tituloCombate; (void)aliados;
    if (enemies.empty()) return -1;

    InputControl::limparBuffer();
    GameWindow::limparMouse();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    InputControl::limparBuffer();

    definirMensagemBanner("SELECIONE UM ALVO [A / D / ENTER]", CorBanner::OURO);

    int alvoSel = 0;

    while (true) {
        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::atualizarTeclas();

        char tecla = InputControl::lerTecla();

        if (tecla == 'l' || tecla == 'L') {
            s_logExpandido = !s_logExpandido;
        }

        renderizarQuadroCombateD2D(currentPlayer, enemies, -1, alvoSel, nullptr, 0, -1, false, false, 4);

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        // Navega entre alvos usando as teclas A e D ou setas para esquerda e direita
        if (tecla == 'a' || tecla == 'A' || tecla == 75) {
            alvoSel = (alvoSel - 1 + (int)enemies.size()) % (int)enemies.size();
        } else if (tecla == 'd' || tecla == 'D' || tecla == 77) {
            alvoSel = (alvoSel + 1) % (int)enemies.size();
        } else if (tecla == '\r' || tecla == '\n' || tecla == ' ') {
            return alvoSel;
        } else if (tecla == 27) { // ESC
            return -1;
        }
    }
}

int TelaCombateRaycaster::obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return obterAlvoAtaque(tituloCombate, enemies, currentPlayer, aliados);
}

int TelaCombateRaycaster::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    if (listaDeEscudos.empty()) return 0;
    std::vector<std::string> nomes;
    for (auto* esc : listaDeEscudos) nomes.push_back(esc->getNameItem());
    int sel = InputControl::lerSelecaoMenuEmPopup("ESCOLHA DE ESCUDO", {"Qual escudo deseja equipar?"}, nomes, Color::YELLOW);
    return sel + 1;
}

void TelaCombateRaycaster::selecionarHUDDeAliado(Character* currentPlayer, const std::vector<Character*>& aliados) { (void)currentPlayer; (void)aliados; }
void TelaCombateRaycaster::notificarInimigosMaisAgeis() { adicionarMensagemFixa("Inimigos sao mais ageis e atacam primeiro!"); }
void TelaCombateRaycaster::notificarTurnoExtra(int dJ, int dI) { (void)dJ; (void)dI; adicionarMensagemFixa("Velocidade superior: Turno Extra!"); }
void TelaCombateRaycaster::notificarDesprevencaoInventario() { adicionarMensagemFixa("Sem item rapido equipado!"); }
void TelaCombateRaycaster::notificarSemEscudos(const std::string& nome) { adicionarMensagemFixa(nome + " tentou defender mas nao tem escudos!"); }
void TelaCombateRaycaster::notificarDesequilibrioDefesa(const std::string& nome) { adicionarMensagemFixa(nome + " teve sua defesa quebrada!"); }
void TelaCombateRaycaster::notificarPosturaDefensiva(const std::string& nome, const std::string& escudo) {
    if (escudo.empty()) adicionarMensagemFixa(nome + " assumiu postura defensiva!");
    else adicionarMensagemFixa(nome + " ergueu o " + escudo + " para defender!");
}
void TelaCombateRaycaster::notificarAcaoInvalida() { adicionarMensagemFixa("Acao Invalida!"); }
void TelaCombateRaycaster::notificarCancelamentoItem() { adicionarMensagemFixa("Uso de item cancelado."); }
void TelaCombateRaycaster::notificarRequisitoNaoAtendido(const std::string& m) { adicionarMensagemFixa(m); }
