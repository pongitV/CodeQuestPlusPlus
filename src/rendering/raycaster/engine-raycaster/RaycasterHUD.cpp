#include "RaycasterHUD.h"
#include "RaycasterWorld.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../maps/control/MapController.h"
#include "../../../entities/races/RaceBase.h"
#include "../../../systems/inventory/Item.h"
#include "../../../core/state/Status.h"
#include <cmath>
#include <chrono>
#include <algorithm>

using namespace std;

static int s_shakeFrames = 0;
Color g_hudBrickColor = Color::RESET;

void RaycasterHUD::gatilharShakeVida() {
    s_shakeFrames = 18;
}

void RaycasterHUD::desenharProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, float jogadorX, float jogadorY, float anguloVisao, const vector<string>& matrizDoMapa, const string& tituloMapa, bool temaFloresta, Character* jogador) {

    desenharBarraStatusProcedural(d2d, screenWidth, screenHeight, jogador);
    desenharControlesProcedural(d2d, screenWidth, screenHeight);
    desenharMinimapaProcedural(d2d, screenWidth, screenHeight, jogadorX, jogadorY, anguloVisao, matrizDoMapa, tituloMapa, temaFloresta);
}

void RaycasterHUD::desenharMinimapaProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, float jogadorX, float jogadorY, float anguloVisao, const vector<string>& matrizDoMapa, const string& tituloMapa, bool temaFloresta) {
    (void)screenWidth; (void)screenHeight; (void)tituloMapa; (void)temaFloresta;
    int larguraMapa = matrizDoMapa.empty() ? 0 : matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();

    float cellSize = 12.0f;
    int minimapCols = 21;
    int minimapRows = 11;
    float minimapWidth = minimapCols * cellSize;
    float minimapHeight = minimapRows * cellSize;
    
    float offsetX = 20.0f;
    float offsetY = 20.0f;

    // Fundo metálico e borda do minimapa
    d2d.preencherRetangulo(offsetX - 2.0f, offsetY - 2.0f, minimapWidth + 4.0f, minimapHeight + 4.0f, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.90f));
    d2d.desenharRetangulo(offsetX - 2.0f, offsetY - 2.0f, minimapWidth + 4.0f, minimapHeight + 4.0f, D2D1::ColorF(0.6f, 0.6f, 0.7f, 1.0f), 2.0f);

    for (int my = 0; my < minimapRows; my++) {
        for (int mx = 0; mx < minimapCols; mx++) {
            int mapX = (int)jogadorX + (mx - minimapCols / 2);
            int mapY = (int)jogadorY + (my - minimapRows / 2);
            
            float px = offsetX + mx * cellSize;
            float py = offsetY + my * cellSize;
            
            if (mapX >= 0 && mapX < larguraMapa && mapY >= 0 && mapY < alturaMapa) {
                char c = matrizDoMapa[mapY][mapX];
                
                // Terreno e Células
                if (c == '#' || c == '=' || c == '|') {
                    // Parede de Pedra / Muro
                    d2d.preencherRetangulo(px + 0.5f, py + 0.5f, cellSize - 1.0f, cellSize - 1.0f, D2D1::ColorF(0.42f, 0.44f, 0.48f, 1.0f));
                } else if (c == '*' || c == 'T' || c == 'F') {
                    // Árvore / Folhagem
                    d2d.preencherRetangulo(px + 0.5f, py + 0.5f, cellSize - 1.0f, cellSize - 1.0f, D2D1::ColorF(0.12f, 0.45f, 0.18f, 1.0f));
                } else if (c == '~') {
                    // Água
                    d2d.preencherRetangulo(px + 0.5f, py + 0.5f, cellSize - 1.0f, cellSize - 1.0f, D2D1::ColorF(0.10f, 0.40f, 0.85f, 1.0f));
                } else if (c == 'D' || c == 'P' || c == 'B' || c == 'N' || c == 'M' || c == 'E' || c == 'S' || c == 'A' || c == 'R') {
                    // Objeto Interativo / Porta / Baú / NPC
                    d2d.preencherRetangulo(px + 0.5f, py + 0.5f, cellSize - 1.0f, cellSize - 1.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f, 1.0f));
                } else if (RaycasterWorld::isEntity(c)) {
                    // Inimigos (Vermelho Carmim)
                    d2d.preencherRetangulo(px + 0.5f, py + 0.5f, cellSize - 1.0f, cellSize - 1.0f, D2D1::ColorF(1.0f, 0.15f, 0.15f, 1.0f));
                } else {
                    // Chão Caminhável
                    d2d.preencherRetangulo(px + 0.5f, py + 0.5f, cellSize - 1.0f, cellSize - 1.0f, D2D1::ColorF(0.14f, 0.15f, 0.18f, 0.85f));
                }

                // Ícone do Jogador no Centro com Indicador de Direção
                if (mx == minimapCols / 2 && my == minimapRows / 2) {
                    d2d.preencherRetangulo(px + 2.0f, py + 2.0f, cellSize - 4.0f, cellSize - 4.0f, D2D1::ColorF(0.0f, 1.0f, 0.5f, 1.0f));
                    float pCx = px + cellSize / 2.0f;
                    float pCy = py + cellSize / 2.0f;
                    float dirX = pCx + std::cos(anguloVisao) * 7.0f;
                    float dirY = pCy + std::sin(anguloVisao) * 7.0f;
                    d2d.preencherRetangulo(dirX - 1.5f, dirY - 1.5f, 3.0f, 3.0f, D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f));
                }
            }
        }
    }
}


void RaycasterHUD::desenharBarraStatusProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, Character* jogador) {
    if (!jogador) return;

    float panelHeight = 140.0f;
    float offsetY = screenHeight - panelHeight;

    // Fundo cinza escuro solido (evita transparência do rejunte quando anda)
    d2d.preencherRetangulo(0, offsetY, screenWidth, panelHeight, D2D1::ColorF(0.12f, 0.12f, 0.14f, 1.0f));
    
    // Parede de pedras medievais (Tijolos procedurais)
    float brickW = 45.0f;
    float brickH = 22.0f;
    int rows = std::ceil(panelHeight / brickH);
    int cols = std::ceil(screenWidth / brickW) + 1;

    float baseR = 0.0f, baseG = 0.0f, baseB = 0.0f;
    switch(g_hudBrickColor) {
        case Color::FUNDO_RED: baseR = 0.25f; break;
        case Color::FUNDO_GREEN: baseG = 0.25f; break;
        case Color::FUNDO_BLUE: baseB = 0.25f; break;
        case Color::FUNDO_MAGENTA: baseR = 0.25f; baseB = 0.25f; break;
        case Color::FUNDO_CYAN: baseG = 0.25f; baseB = 0.25f; break;
        default: break;
    }

    for (int r = 0; r < rows; ++r) {
        float startX = (r % 2 == 0) ? 0.0f : -(brickW / 2.0f);
        for (int c = 0; c < cols; ++c) {
            float bx = startX + c * brickW;
            float by = offsetY + r * brickH;
            float shade = 0.14f + ((r * 7 + c * 13) % 11) * 0.005f;
            
            d2d.preencherRetangulo(bx + 1.0f, by + 1.0f, brickW - 2.0f, brickH - 2.0f, D2D1::ColorF(std::min(1.0f, shade + baseR), std::min(1.0f, shade + baseG), std::min(1.0f, shade + baseB), 1.0f));
            d2d.preencherRetangulo(bx + 1.0f, by + 1.0f, brickW - 2.0f, 1.0f, D2D1::ColorF(std::min(1.0f, shade + 0.1f + baseR), std::min(1.0f, shade + 0.1f + baseG), std::min(1.0f, shade + 0.1f + baseB), 1.0f));
            d2d.preencherRetangulo(bx + 1.0f, by + 1.0f, 1.0f, brickH - 2.0f, D2D1::ColorF(std::min(1.0f, shade + 0.1f + baseR), std::min(1.0f, shade + 0.1f + baseG), std::min(1.0f, shade + 0.1f + baseB), 1.0f));
            d2d.preencherRetangulo(bx + brickH - 2.0f, by + brickH - 2.0f, brickW - 2.0f, 1.0f, D2D1::ColorF(std::max(0.0f, shade - 0.1f + baseR), std::max(0.0f, shade - 0.1f + baseG), std::max(0.0f, shade - 0.1f + baseB), 1.0f));
            d2d.preencherRetangulo(bx + brickW - 2.0f, by + 1.0f, 1.0f, brickH - 2.0f, D2D1::ColorF(std::max(0.0f, shade - 0.1f + baseR), std::max(0.0f, shade - 0.1f + baseG), std::max(0.0f, shade - 0.1f + baseB), 1.0f));
        }
    }
    
    // Borda superior ornamental (Madeira/Ferro)
    d2d.preencherRetangulo(0, offsetY, screenWidth, 8.0f, D2D1::ColorF(0.2f, 0.2f, 0.2f, 1.0f));
    d2d.preencherRetangulo(0, offsetY + 1.0f, screenWidth, 2.0f, D2D1::ColorF(0.4f, 0.4f, 0.4f, 1.0f));
    d2d.preencherRetangulo(0, offsetY + 6.0f, screenWidth, 2.0f, D2D1::ColorF(0.1f, 0.1f, 0.1f, 1.0f));
    for (int i = 20; i < screenWidth; i += 60) {
        d2d.preencherRetangulo((float)i, offsetY + 3.0f, 3.0f, 3.0f, D2D1::ColorF(0.05f, 0.05f, 0.05f, 1.0f));
        d2d.preencherRetangulo((float)i + 1.0f, offsetY + 4.0f, 1.0f, 1.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f, 1.0f));
    }

    // ═══════════════════════════════════════════════════════════════════
    // PAINÉIS LADO ESQUERDO: ATRIBUTOS E STATUS & CURA
    // ═══════════════════════════════════════════════════════════════════
    // 1. Caixa de Atributos (Extremo Esquerdo)
    float attrBoxX = 25.0f;
    float attrBoxY = offsetY + 15.0f;
    float attrBoxW = 195.0f;
    float attrBoxH = 110.0f;

    d2d.preencherRetangulo(attrBoxX, attrBoxY, attrBoxW, attrBoxH, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.55f));
    d2d.desenharRetangulo(attrBoxX, attrBoxY, attrBoxW, attrBoxH, D2D1::ColorF(0.4f, 0.4f, 0.4f, 0.9f), 1.0f);

    float attrTextY = attrBoxY + 8.0f;
    d2d.desenharTexto(L"--- ATRIBUTOS ---", attrBoxX + 28.0f, attrTextY, D2D1::ColorF(0.85f, 0.85f, 0.85f), 14.0f);

    float col1X = attrBoxX + 12.0f;
    float col2X = attrBoxX + 102.0f;
    float row1Y = attrTextY + 24.0f;
    float row2Y = attrTextY + 46.0f;
    float row3Y = attrTextY + 68.0f;

    d2d.desenharTexto(L"FOR: " + to_wstring(jogador->getStrength()), col1X, row1Y, D2D1::ColorF(1.0f, 0.6f, 0.6f), 13.0f);
    d2d.desenharTexto(L"INT: " + to_wstring(jogador->getInteligencia()), col2X, row1Y, D2D1::ColorF(0.4f, 0.8f, 1.0f), 13.0f);
    d2d.desenharTexto(L"DES: " + to_wstring(jogador->getDexterity()), col1X, row2Y, D2D1::ColorF(0.6f, 1.0f, 0.6f), 13.0f);
    d2d.desenharTexto(L"SAB: " + to_wstring(jogador->getWisdom()), col2X, row2Y, D2D1::ColorF(1.0f, 0.84f, 0.0f), 13.0f);
    d2d.desenharTexto(L"CON: " + to_wstring(jogador->getConstitution()), col1X, row3Y, D2D1::ColorF(0.0f, 1.0f, 1.0f), 13.0f);
    d2d.desenharTexto(L"RES: " + to_wstring(jogador->getResistance()), col2X, row3Y, D2D1::ColorF(0.6f, 0.6f, 1.0f), 13.0f);

    // 2. Caixa de Cura & Status (Ao lado de Atributos, Lado Esquerdo - Expande Dinamicamente)
    Item* pocao = jogador->obterConsumivelRapido();
    std::string nomePocao = pocao ? pocao->getNameItem() + " (" + std::to_string(jogador->obterInventario()->contarItem(pocao->getNameItem())) + "x)" : "Nenhuma";
    wstring pocaoText = L"Cura: " + wstring(nomePocao.begin(), nomePocao.end());

    // Coleta de Buffs e Debuffs
    std::wstring buffsStr = L"Nenhum";
    std::wstring debuffsStr = L"Nenhum";
    std::vector<std::wstring> bList;
    std::vector<std::wstring> dList;

    if (jogador->possuiEfeito(EfeitoID::GritoDeGuerra)) bList.push_back(L"Grito Guerra");
    if (jogador->possuiEfeito(EfeitoID::MiraCerteira)) bList.push_back(L"Mira Certeira");
    if (jogador->possuiEfeito(EfeitoID::BuffAtributos)) bList.push_back(L"Atributos+");
    if (jogador->possuiEfeito(EfeitoID::Inviolavel)) bList.push_back(L"Inviolável");

    if (jogador->possuiEfeito(EfeitoID::Sangramento)) dList.push_back(L"Sangramento");
    if (jogador->possuiEfeito(EfeitoID::Lentidao)) dList.push_back(L"Lentidão");
    if (jogador->possuiEfeito(EfeitoID::Fraqueza)) dList.push_back(L"Fraqueza");
    if (jogador->possuiEfeito(EfeitoID::Atordoamento)) dList.push_back(L"Atordoado");
    if (jogador->possuiEfeito(EfeitoID::Necrose)) dList.push_back(L"Necrose");

    if (!bList.empty()) {
        buffsStr = L"";
        for (size_t i = 0; i < bList.size(); ++i) buffsStr += bList[i] + (i + 1 < bList.size() ? L", " : L"");
    }
    if (!dList.empty()) {
        debuffsStr = L"";
        for (size_t i = 0; i < dList.size(); ++i) debuffsStr += dList[i] + (i + 1 < dList.size() ? L", " : L"");
    }

    size_t maxStatusLen = std::max({ pocaoText.size(), (L"Buffs: " + buffsStr).size(), (L"Debuffs: " + debuffsStr).size() });
    float statusBoxW = std::max(215.0f, 65.0f + static_cast<float>(maxStatusLen) * 7.5f);
    float statusBoxX = attrBoxX + attrBoxW + 15.0f;
    float statusBoxY = offsetY + 15.0f;
    float statusBoxH = 110.0f;

    d2d.preencherRetangulo(statusBoxX, statusBoxY, statusBoxW, statusBoxH, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.55f));
    d2d.desenharRetangulo(statusBoxX, statusBoxY, statusBoxW, statusBoxH, D2D1::ColorF(0.4f, 0.4f, 0.4f, 0.9f), 1.0f);

    float statusTextY = statusBoxY + 8.0f;
    d2d.desenharTexto(L"--- CURA & STATUS ---", statusBoxX + (statusBoxW / 2.0f) - 65.0f, statusTextY, D2D1::ColorF(0.85f, 0.85f, 0.85f), 14.0f);

    d2d.desenharTexto(pocaoText, statusBoxX + 12.0f, statusTextY + 24.0f, D2D1::ColorF(0.4f, 1.0f, 0.4f), 12.0f);
    d2d.desenharTexto(L"Buffs: " + buffsStr, statusBoxX + 12.0f, statusTextY + 46.0f, bList.empty() ? D2D1::ColorF(0.7f, 0.7f, 0.7f) : D2D1::ColorF(0.2f, 1.0f, 0.5f), 12.0f);
    d2d.desenharTexto(L"Debuffs: " + debuffsStr, statusBoxX + 12.0f, statusTextY + 68.0f, dList.empty() ? D2D1::ColorF(0.7f, 0.7f, 0.7f) : D2D1::ColorF(1.0f, 0.3f, 0.3f), 12.0f);

    // ═══════════════════════════════════════════════════════════════════
    // PAINÉIS LADO DIREITO: EQUIPAMENTOS E CONTROLES
    // ═══════════════════════════════════════════════════════════════════
    // 3. Controles Box (Extremo Direito)
    float ctrlBoxX = screenWidth - 245.0f;
    float ctrlBoxY = offsetY + 15.0f;
    float ctrlBoxW = 225.0f;
    float ctrlBoxH = 110.0f;
    
    d2d.preencherRetangulo(ctrlBoxX, ctrlBoxY, ctrlBoxW, ctrlBoxH, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.55f));
    d2d.desenharRetangulo(ctrlBoxX, ctrlBoxY, ctrlBoxW, ctrlBoxH, D2D1::ColorF(0.4f, 0.4f, 0.4f, 0.9f), 1.0f);
    
    float ctrlTextX = ctrlBoxX + 12.0f;
    float ctrlTextY = ctrlBoxY + 8.0f;
    
    d2d.desenharTexto(L"--- CONTROLES ---", ctrlBoxX + 32.0f, ctrlTextY, D2D1::ColorF(0.85f, 0.85f, 0.85f), 14.0f);
    d2d.desenharTexto(L"[WASD] Mover | [ESC] Sair", ctrlTextX, ctrlTextY + 24.0f, D2D1::ColorF(0.65f, 0.65f, 0.65f), 12.0f);
    d2d.desenharTexto(L"[I] Inventário | [C] Ficha", ctrlTextX, ctrlTextY + 46.0f, D2D1::ColorF(0.65f, 0.65f, 0.65f), 12.0f);
    d2d.desenharTexto(L"[M] Mapa | [B] Diário", ctrlTextX, ctrlTextY + 68.0f, D2D1::ColorF(0.65f, 0.65f, 0.65f), 12.0f);

    // 4. Equipamentos Box (Dinamica ao lado dos Controles)
    std::wstring wpName = L"Punhos";
    if (Item* wp = jogador->obterArma()) { std::string s = wp->getNameItem(); wpName = std::wstring(s.begin(), s.end()); }
    
    std::wstring arName = L"Trapos";
    if (Item* ar = jogador->obterArmadura()) { std::string s = ar->getNameItem(); arName = std::wstring(s.begin(), s.end()); }
    
    std::wstring shName = L"Nenhum";
    if (Item* sh = jogador->obterEscudo()) { std::string s = sh->getNameItem(); shName = std::wstring(s.begin(), s.end()); }

    size_t maxEqLen = std::max({ wpName.size(), shName.size(), arName.size() });
    float eqBoxW = std::max(210.0f, 70.0f + static_cast<float>(maxEqLen) * 8.0f);
    float eqBoxX = ctrlBoxX - eqBoxW - 15.0f;
    float eqBoxY = offsetY + 15.0f;
    float eqBoxH = 110.0f;

    d2d.preencherRetangulo(eqBoxX, eqBoxY, eqBoxW, eqBoxH, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.55f));
    d2d.desenharRetangulo(eqBoxX, eqBoxY, eqBoxW, eqBoxH, D2D1::ColorF(0.4f, 0.4f, 0.4f, 0.9f), 1.0f);

    float eqTextY = eqBoxY + 8.0f;
    d2d.desenharTexto(L"--- EQUIPAMENTOS ---", eqBoxX + (eqBoxW / 2.0f) - 65.0f, eqTextY, D2D1::ColorF(0.85f, 0.85f, 0.85f), 14.0f);

    float eqTextX = eqBoxX + 12.0f;
    d2d.desenharTexto(L"Arma: " + wpName, eqTextX, eqTextY + 24.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), 12.0f);
    d2d.desenharTexto(L"Escudo: " + shName, eqTextX, eqTextY + 46.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), 12.0f);
    d2d.desenharTexto(L"Traje: " + arName, eqTextX, eqTextY + 68.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), 12.0f);

    // ═══════════════════════════════════════════════════════════════════
    // ÁREA CENTRAL DO HUD: ESCUDO OVAL-RETANGULAR METÁLICO (TEXTURIZADO)
    // ═══════════════════════════════════════════════════════════════════
    float centerW = 440.0f; // Mais largo que alto
    float centerH = 140.0f;
    float centerX = (screenWidth - centerW) / 2.0f;
    float pedY = offsetY - 16.0f;

    if (d2d.obterRenderTarget()) {
        d2d.preencherRetangulo(centerX, pedY, centerW, centerH, D2D1::ColorF(0.15f, 0.17f, 0.20f, 1.0f));

        // 2. Textura Procedural de Aço Escovado (Linhas Metálicas Horizontais com Shading)
        for (float y = pedY + 3.0f; y < pedY + centerH - 3.0f; y += 3.5f) {
            float distFromCenter = std::abs((y - (pedY + centerH / 2.0f)) / (centerH / 2.0f));
            float specHighlight = std::pow(1.0f - distFromCenter, 2.0f) * 0.08f;
            float grainShade = 0.14f + specHighlight + (std::sin(y * 0.5f) + 1.0f) * 0.025f;

            float curveInset = distFromCenter * distFromCenter * 16.0f;
            float lineX1 = centerX + 5.0f + curveInset;
            float lineX2 = centerX + centerW - 5.0f - curveInset;

            if (lineX2 > lineX1) {
                d2d.preencherRetangulo(lineX1, y, lineX2 - lineX1, 1.5f, D2D1::ColorF(grainShade, grainShade + 0.02f, grainShade + 0.04f, 0.95f));
            }
        }

        // 3. Brilho Reflexivo Diagonal Escovado (Specular Metallic Streak)
        for (float offset = -60.0f; offset < centerW + 60.0f; offset += 8.0f) {
            float specAlpha = std::max(0.0f, 0.035f - std::abs(offset - centerW * 0.45f) * 0.0005f);
            if (specAlpha > 0.005f) {
                d2d.desenharLinha(centerX + offset, pedY + 4.0f, centerX + offset + 35.0f, pedY + centerH - 4.0f, D2D1::ColorF(0.85f, 0.90f, 0.98f, specAlpha), 3.5f);
            }
        }

        // 4. Bisel Duplo Metálico Cinza Escuro
        d2d.desenharRetangulo(centerX, pedY, centerW, centerH, D2D1::ColorF(0.30f, 0.32f, 0.36f, 1.0f), 3.0f);
        d2d.desenharRetangulo(centerX + 3.5f, pedY + 3.5f, centerW - 7.0f, centerH - 7.0f, D2D1::ColorF(0.30f, 0.32f, 0.36f, 1.0f), 1.5f);

        // 5. Rebites Metálicos de Ferro nos Cantos do Escudo Oval
        float rX[] = { centerX + 22.0f, centerX + centerW - 22.0f, centerX + 22.0f, centerX + centerW - 22.0f };
        float rY[] = { pedY + 16.0f, pedY + 16.0f, pedY + centerH - 16.0f, pedY + centerH - 16.0f };
        for (int i = 0; i < 4; ++i) {
            d2d.preencherElipse(rX[i], rY[i], 3.5f, 3.5f, D2D1::ColorF(0.08f, 0.08f, 0.10f, 1.0f));
            d2d.preencherElipse(rX[i] - 1.0f, rY[i] - 1.0f, 1.5f, 1.5f, D2D1::ColorF(0.50f, 0.54f, 0.60f, 1.0f));
        }
    }


    // 1. Barra de Vida (SISTEMA DE SHAKE, VIDRO QUEBRADO & SANGUE VAZANDO)
    float hpPercent = (float)jogador->obterVida() / (float)std::max(1, jogador->obterVidaMaxima());
    float barW = 340.0f;
    float barH = 18.0f;
    
    // Shake violento ao receber dano
    float shakeOffsetX = 0.0f;
    float shakeOffsetY = 0.0f;
    if (s_shakeFrames > 0) {
        s_shakeFrames--;
        shakeOffsetX = ((float)(std::rand() % 17) - 8.0f) * 1.8f;
        shakeOffsetY = ((float)(std::rand() % 13) - 6.0f) * 1.4f;
    }

    float hpX = (screenWidth - barW) / 2.0f + shakeOffsetX;
    float hpY = offsetY + 6.0f + shakeOffsetY;
    
    // Fundo Carvão e Preenchimento Vermelho Sangue
    d2d.preencherRetangulo(hpX, hpY, barW, barH, D2D1::ColorF(0.18f, 0.04f, 0.04f, 0.9f));
    d2d.preencherRetangulo(hpX, hpY, barW * std::clamp(hpPercent, 0.0f, 1.0f), barH, D2D1::ColorF(0.82f, 0.05f, 0.05f, 1.0f));

    // ESTADOS DO VIDRO DA BARRA DE VIDA ( < 30% Quebrado/Sangue, 30-60% Rachado, > 60% Normal )
    if (hpPercent > 0.60f) {
        // Normal: Brilho reflexivo limpo do vidro
        d2d.preencherRetangulo(hpX + 2.0f, hpY + 2.0f, barW - 4.0f, 3.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.35f));
        d2d.desenharRetangulo(hpX, hpY, barW, barH, D2D1::ColorF(0.85f, 0.85f, 0.90f, 0.9f), 1.5f);
    } else if (hpPercent >= 0.30f) {
        // 30% a 60%: Vidro Rachado (Fissuras Diagonais)
        d2d.preencherRetangulo(hpX + 2.0f, hpY + 2.0f, barW - 4.0f, 3.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f));
        d2d.desenharRetangulo(hpX, hpY, barW, barH, D2D1::ColorF(0.9f, 0.8f, 0.6f, 0.95f), 1.5f);
        
        // Fissuras de vidro rachado (Usa brush cacheado do D2DRenderer)
        D2D1_COLOR_F crackColor = D2D1::ColorF(0.95f, 0.98f, 1.0f, 0.85f);
        d2d.desenharLinha(hpX + barW * 0.22f, hpY + 1.0f, hpX + barW * 0.28f, hpY + barH - 1.0f, crackColor, 1.5f);
        d2d.desenharLinha(hpX + barW * 0.25f, hpY + 8.0f, hpX + barW * 0.34f, hpY + 14.0f, crackColor, 1.2f);
        d2d.desenharLinha(hpX + barW * 0.68f, hpY + barH - 1.0f, hpX + barW * 0.74f, hpY + 2.0f, crackColor, 1.5f);
        d2d.desenharLinha(hpX + barW * 0.71f, hpY + 7.0f, hpX + barW * 0.80f, hpY + 12.0f, crackColor, 1.2f);
    } else {
        // < 30%: Vidro Quebrado Severo + Sangue Vazando pela Barra
        d2d.desenharRetangulo(hpX, hpY, barW, barH, D2D1::ColorF(1.0f, 0.15f, 0.15f, 0.95f), 2.0f);
        
        // Fissuras profundas de vidro destruído (Usa brush cacheado do D2DRenderer)
        D2D1_COLOR_F deepCrackColor = D2D1::ColorF(1.0f, 0.9f, 0.9f, 0.95f);
        d2d.desenharLinha(hpX + barW * 0.15f, hpY + 1.0f, hpX + barW * 0.24f, hpY + barH - 1.0f, deepCrackColor, 1.8f);
        d2d.desenharLinha(hpX + barW * 0.20f, hpY + 6.0f, hpX + barW * 0.36f, hpY + 15.0f, deepCrackColor, 1.4f);
        d2d.desenharLinha(hpX + barW * 0.50f, hpY + 1.0f, hpX + barW * 0.58f, hpY + barH - 1.0f, deepCrackColor, 1.8f);
        d2d.desenharLinha(hpX + barW * 0.75f, hpY + barH - 1.0f, hpX + barW * 0.85f, hpY + 2.0f, deepCrackColor, 1.8f);

        // VAZAMENTO DE SANGUE (Gotas escorrendo para baixo)
        float dripPositions[] = { 0.12f, 0.28f, 0.45f, 0.62f, 0.78f, 0.91f };
        float dripHeights[] = { 14.0f, 22.0f, 18.0f, 26.0f, 16.0f, 20.0f };
        for (int b = 0; b < 6; ++b) {
            float dripX = hpX + barW * dripPositions[b];
            float dripH = dripHeights[b] + std::sin(GetTickCount64() * 0.006f + b * 1.5f) * 3.5f;
            d2d.preencherRetangulo(dripX, hpY + barH, 3.5f, dripH, D2D1::ColorF(0.75f, 0.02f, 0.02f, 0.92f));
            
            // Gotas arredondadas de sangue nas pontas (Usa brush cacheado do D2DRenderer)
            d2d.preencherElipse(dripX + 1.75f, hpY + barH + dripH, 3.0f, 4.0f, D2D1::ColorF(0.90f, 0.02f, 0.02f, 0.98f));
        }
    }
    
    // Texto do HP (Branco e Centralizado dentro da Barra)
    wstring hpText = L"HP: " + to_wstring(jogador->obterVida()) + L" / " + to_wstring(jogador->obterVidaMaxima());
    float hpTextX = (screenWidth / 2.0f) - (hpText.size() * 3.8f);
    d2d.desenharTexto(hpText, hpTextX, hpY + 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 13.0f);

    // 2. Barra de XP (PULSANDO EM LUZ BLUE QUANDO CHEIA)
    int xpAtual = jogador->getXpAtual();
    int xpProx = jogador->getXpParaSubir();
    bool xpCheia = (xpAtual >= xpProx) || (jogador && jogador->podeSubirDeNivel());
    float xpPercent = (float)xpAtual / std::max(1, xpProx);
    if (xpPercent > 1.0f) xpPercent = 1.0f;

    float xpY = hpY + 24.0f;

    if (xpCheia) {
        // Efeito de pulso em Luz Azul Neon quando a barra de XP está cheia
        float pulse = (std::sin(GetTickCount64() * 0.007f) + 1.0f) * 0.5f; // 0.0 a 1.0
        d2d.preencherRetangulo(hpX, xpY, barW, barH, D2D1::ColorF(0.02f, 0.12f + pulse * 0.2f, 0.35f + pulse * 0.3f, 0.95f));
        d2d.preencherRetangulo(hpX, xpY, barW, barH, D2D1::ColorF(0.0f, 0.60f + pulse * 0.40f, 1.0f, 1.0f)); // Azul Brilhante Pulsante
        d2d.desenharRetangulo(hpX, xpY, barW, barH, D2D1::ColorF(0.4f + pulse * 0.6f, 0.8f + pulse * 0.2f, 1.0f, 1.0f), 2.0f + pulse * 1.5f);

        wstring xpText = L"LEVEL UP PRONTO! [ XP: " + to_wstring(xpAtual) + L" / " + to_wstring(xpProx) + L" ]";
        float xpTextX = (screenWidth / 2.0f) - (xpText.size() * 3.8f);
        d2d.desenharTexto(xpText, xpTextX, xpY + 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 13.0f);
    } else {
        d2d.preencherRetangulo(hpX, xpY, barW, barH, D2D1::ColorF(0.04f, 0.1f, 0.2f, 0.9f));
        d2d.preencherRetangulo(hpX, xpY, barW * xpPercent, barH, D2D1::ColorF(0.0f, 0.55f, 0.95f, 1.0f)); // Azul XP Normal
        d2d.desenharRetangulo(hpX, xpY, barW, barH, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.9f), 1.5f);

        wstring xpText = L"XP: " + to_wstring(xpAtual) + L" / " + to_wstring(xpProx);
        float xpTextX = (screenWidth / 2.0f) - (xpText.size() * 3.8f);
        d2d.desenharTexto(xpText, xpTextX, xpY + 1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 13.0f);
    }


    // 3. NOME DO JOGADOR (Texto Grande Centralizado)
    std::string nomeStr = jogador->getName();
    wstring nameText = wstring(nomeStr.begin(), nomeStr.end());
    float nameY = xpY + 24.0f;
    float nameX = (screenWidth / 2.0f) - (nameText.size() * 5.5f);
    d2d.desenharTexto(nameText, nameX, nameY, D2D1::ColorF(1.0f, 0.85f, 0.0f), 19.0f); // Texto Grande Dourado

    // 4. Classe e Raça ("Classe" / "Raça") Centralizados
    std::string racaStr = jogador->obterRaca() ? jogador->obterRaca()->getRaceName() : "Humano";
    std::string classeStr = jogador->getNameClasse();

    wstring classRaceText = L"Classe: \"" + wstring(classeStr.begin(), classeStr.end()) +
                            L"\" / Raça: \"" + wstring(racaStr.begin(), racaStr.end()) +
                            L"\" (Nível " + to_wstring(jogador->getLevel()) + L")";
    float infoY = nameY + 24.0f;
    float infoX = (screenWidth / 2.0f) - (classRaceText.size() * 3.6f);
    d2d.desenharTexto(classRaceText, infoX, infoY, D2D1::ColorF(0.9f, 0.9f, 0.9f), 13.0f);

    // 5. Ouro Info (Centralizado no Rodapé)
    wstring goldText = L"Ouro: " + to_wstring(jogador->obterInventario()->obterOuro()) + L"g";
    float goldX = (screenWidth / 2.0f) - (goldText.size() * 3.5f);
    d2d.desenharTexto(goldText, goldX, infoY + 18.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), 13.0f);
}

void RaycasterHUD::desenharControlesProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight) {
    (void)d2d; (void)screenWidth; (void)screenHeight;
}

void RaycasterHUD::desenharOpcoesCombateProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, int opcaoSelecionada, const std::string& mensagemBanner, CorBanner corBanner, const std::string& mensagemBannerLinha2) {
    float panelH = 50.0f;
    float panelW = screenWidth * 0.70f;
    float panelX = (screenWidth - panelW) / 2.0f;
    float panelY = screenHeight - 140.0f - panelH - 5.0f;

    // Fundo Tijolos Castelo (Procedural)
    d2d.preencherRetangulo(panelX, panelY, panelW, panelH, D2D1::ColorF(0.12f, 0.12f, 0.14f, 1.0f));

    float brickW = 40.0f;
    float brickH = 20.0f;
    int rows = std::ceil(panelH / brickH);
    int cols = std::ceil(panelW / brickW) + 1;

    float baseR = 0.0f, baseG = 0.0f, baseB = 0.0f;
    switch(g_hudBrickColor) {
        case Color::FUNDO_RED: baseR = 0.25f; break;
        case Color::FUNDO_GREEN: baseG = 0.25f; break;
        case Color::FUNDO_BLUE: baseB = 0.25f; break;
        case Color::FUNDO_MAGENTA: baseR = 0.25f; baseB = 0.25f; break;
        case Color::FUNDO_CYAN: baseG = 0.25f; baseB = 0.25f; break;
        default: break;
    }

    for (int r = 0; r < rows; ++r) {
        float startX = (r % 2 == 0) ? 0.0f : -(brickW / 2.0f);
        for (int c = 0; c < cols; ++c) {
            float bx = panelX + startX + c * brickW;
            float by = panelY + r * brickH;
            if (bx + brickW < panelX || bx > panelX + panelW) continue;
            float shade = 0.15f + ((r * 5 + c * 11) % 9) * 0.006f;

            float drawW = std::min(bx + brickW - 2.0f, panelX + panelW - 1.0f) - std::max(bx + 1.0f, panelX + 1.0f);
            if (drawW <= 0) continue;
            float drawX = std::max(bx + 1.0f, panelX + 1.0f);

            d2d.preencherRetangulo(drawX, by + 1.0f, drawW, brickH - 2.0f, D2D1::ColorF(std::min(1.0f, shade + baseR), std::min(1.0f, shade + baseG), std::min(1.0f, shade + baseB), 1.0f));
            if (drawX == bx + 1.0f) d2d.preencherRetangulo(drawX, by + 1.0f, drawW, 1.0f, D2D1::ColorF(std::min(1.0f, shade + 0.1f + baseR), std::min(1.0f, shade + 0.1f + baseG), std::min(1.0f, shade + 0.1f + baseB), 1.0f));
            if (drawX == bx + 1.0f) d2d.preencherRetangulo(drawX, by + 1.0f, 1.0f, brickH - 2.0f, D2D1::ColorF(std::min(1.0f, shade + 0.1f + baseR), std::min(1.0f, shade + 0.1f + baseG), std::min(1.0f, shade + 0.1f + baseB), 1.0f));
        }
    }

    // Moldura Ornamental de Ferro / Madeira
    d2d.desenharRetangulo(panelX, panelY, panelW, panelH, D2D1::ColorF(0.4f, 0.4f, 0.5f, 1.0f), 2.0f);
    d2d.preencherRetangulo(panelX, panelY, panelW, 4.0f, D2D1::ColorF(0.25f, 0.25f, 0.3f, 1.0f));
    d2d.preencherRetangulo(panelX, panelY + panelH - 4.0f, panelW, 4.0f, D2D1::ColorF(0.25f, 0.25f, 0.3f, 1.0f));

    // Renderizar as 5 Opções de Combate ou o Aviso Customizado / PRESSIONE ENTER no Tema da HUD
    if (opcaoSelecionada == -1) {
        float pulse = (std::sin(GetTickCount64() * 0.005f) + 1.0f) * 0.5f;
        D2D1_COLOR_F goldPulse = D2D1::ColorF(1.0f, 0.84f + pulse * 0.16f, pulse * 0.35f);
        D2D1_COLOR_F bgGlow = D2D1::ColorF(0.12f, 0.08f, 0.02f, 0.85f);

        switch (corBanner) {
            case CorBanner::YELLOW:
                goldPulse = D2D1::ColorF(1.0f, 1.0f, 0.2f + pulse * 0.3f);
                bgGlow = D2D1::ColorF(0.16f, 0.16f, 0.02f, 0.90f);
                break;
            case CorBanner::ORANGE:
                goldPulse = D2D1::ColorF(1.0f, 0.50f + pulse * 0.20f, 0.0f);
                bgGlow = D2D1::ColorF(0.20f, 0.06f, 0.01f, 0.90f);
                break;
            case CorBanner::GREEN_CLARO:
                goldPulse = D2D1::ColorF(0.2f, 1.0f, 0.3f + pulse * 0.2f);
                bgGlow = D2D1::ColorF(0.02f, 0.18f, 0.04f, 0.90f);
                break;
            case CorBanner::GREEN_ESCURO:
                goldPulse = D2D1::ColorF(0.1f, 0.65f, 0.2f + pulse * 0.15f);
                bgGlow = D2D1::ColorF(0.01f, 0.10f, 0.02f, 0.90f);
                break;
            case CorBanner::OURO:
            default:
                break;
        }
        
        d2d.preencherRetangulo(panelX + 4.0f, panelY + 4.0f, panelW - 8.0f, panelH - 8.0f, bgGlow);
        d2d.desenharRetangulo(panelX + 4.0f, panelY + 4.0f, panelW - 8.0f, panelH - 8.0f, goldPulse, 2.0f);
        
        std::string textoExibir = mensagemBanner.empty() ? "[ PRESSIONE ENTER PARA CONTINUAR ]" : mensagemBanner;
        if (!textoExibir.empty() && textoExibir.front() != '[') {
            textoExibir = "[ " + textoExibir + " ]";
        }

        if (!mensagemBannerLinha2.empty()) {
            std::string linha2 = mensagemBannerLinha2;
            if (!linha2.empty() && linha2.front() != '[') {
                linha2 = "[ " + linha2 + " ]";
            }
            std::wstring t1 = std::wstring(textoExibir.begin(), textoExibir.end());
            std::wstring t2 = std::wstring(linha2.begin(), linha2.end());
            d2d.desenharTexto(t1, panelX + (panelW / 2.0f) - (t1.size() * 4.4f), panelY + 6.0f, goldPulse, 14.0f);
            d2d.desenharTexto(t2, panelX + (panelW / 2.0f) - (t2.size() * 4.4f), panelY + 26.0f, goldPulse, 14.0f);
        } else {
            std::wstring promptText = std::wstring(textoExibir.begin(), textoExibir.end());
            d2d.desenharTexto(promptText, panelX + (panelW / 2.0f) - (promptText.size() * 5.2f), panelY + 12.0f, goldPulse, 17.0f);
        }
    } else {
        std::vector<std::wstring> opcoes = {L"1. ATACAR", L"2. HABILIDADE", L"3. DEFENDER", L"4. ITENS", L"5. DIÁRIO"};
        float itemW = panelW / opcoes.size();

        for (size_t i = 0; i < opcoes.size(); ++i) {
            bool sel = ((int)i == opcaoSelecionada);
            float itemX = panelX + i * itemW;

            if (sel) {
                d2d.preencherRetangulo(itemX + 4.0f, panelY + 4.0f, itemW - 8.0f, panelH - 8.0f, D2D1::ColorF(0.4f, 0.3f, 0.05f, 0.6f));
                d2d.desenharRetangulo(itemX + 4.0f, panelY + 4.0f, itemW - 8.0f, panelH - 8.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f, 1.0f), 2.0f);
            }

            std::wstring text = sel ? (L"> " + opcoes[i] + L" <") : opcoes[i];
            D2D1_COLOR_F color = sel ? D2D1::ColorF(1.0f, 0.84f, 0.0f) : D2D1::ColorF(0.8f, 0.8f, 0.8f);

            d2d.desenharTexto(text, itemX + (itemW / 2.0f) - (text.size() * 4.5f), panelY + 14.0f, color, 16.0f);
        }
    }
}
