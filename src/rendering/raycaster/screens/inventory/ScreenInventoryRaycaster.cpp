#include "ScreenInventoryRaycaster.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../core/window/GameWindow.h"
#include <iostream>
#include <vector>
#include "../../../../core/utils/Color.h"
#include "../../../../systems/inventory/Item.h"
#include "../../../../entities/character/Character.h"
#include "../../../../ui/screens/ScreenBase.h"
#include "../../../../ui/screens/inventory/ScreenInventoryLayout.h"
#include "../../engine-raycaster/Raycaster.h"
#include "../utils/MenuRaycasterUtils.h"

void TelaInventarioRaycaster::displayCabecalho(bool, int startY) {
    int larguraConsole = 120;
    
    // Desenha o logo do INVENTARIO
    int logoHeight = ArtesInventario::logoInventario.size();
    int logoY = startY > 0 ? (startY - 1 - logoHeight) : 2;
    if (logoY < 0) logoY = 0;
    
    int compVisualLogo = 0;
    for (const auto& linha : ArtesInventario::logoInventario) {
        int comp = (int)(linha).length();
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = (larguraConsole - compVisualLogo) / 2;
    if (logoX < 0) logoX = 0;
    
    // Apenas desenha o logo sobre a tela atual, pulando espacos para nao pintar fundo preto
    std::string corTitulo = "";
    for (int i = 0; i < (int)ArtesInventario::logoInventario.size(); ++i) {
        const std::string& linha = ArtesInventario::logoInventario[i];
        
        std::string buffer = linha;
    }
}

void TelaInventarioRaycaster::displayCaixaEquipados(Character*) {}
void TelaInventarioRaycaster::displayDetalheItem(Item*) {}

void TelaInventarioRaycaster::renderizarMenu(const std::vector<std::string>& linhas, const std::string& titulo, int selecaoAtual, int& outW, int& outH) {
    if (outH > 0 && outW > 0) {
        Raycaster::restaurarUltimoQuadro();
    }
    
    std::vector<GrupoCorUI> paleta = {
        {"█", 255, 150, 0},
        {"_", 255, 150, 0},
        {"|", 255, 150, 0},
        {"-", 255, 150, 0},
        {"/", 255, 150, 0},
        {"\\", 255, 150, 0}
    };
    
    auto renderCb = [&](UIDynamicBox& box, int selA, float lw, float lh) {
        float currentY = 30.0f;
        box.AddPixelArt(ArtesInventario::logoInventario, paleta, lw / 2.0f, currentY, 1.0f);
        currentY += ArtesInventario::logoInventario.size() * 16.0f + 20.0f;
        
        std::wstring wTit = MenuRaycasterUtils::utf8_to_wstring(titulo);
        box.AddText(wTit, lw/2.0f, currentY, 20.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f), true);
        currentY += 40.0f;
        
        int interactiveIdx = 0;
        std::string strBolso = "BOLSO:";
        
        for (size_t i = 0; i < linhas.size(); ++i) {
            std::string line = linhas[i];
            
            // Remove sequencias ANSI residuais
            size_t pos = 0;
            while ((pos = line.find("[")) != std::string::npos) {
                size_t endPos = line.find('m', pos);
                if (endPos != std::string::npos) line.erase(pos, endPos - pos + 1);
                else break;
            }
            
            if (line.empty() || line.find(strBolso) != std::string::npos || line.find("   ") == 0) {
                std::wstring wLine = MenuRaycasterUtils::utf8_to_wstring(line);
                box.AddText(wLine, lw/2.0f, currentY, 16.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f), true);
                currentY += 25.0f;
            } else {
                std::wstring wOp = MenuRaycasterUtils::utf8_to_wstring(line);
                std::wstring text = (interactiveIdx == selecaoAtual) ? (L"> " + wOp) : (L"  " + wOp);
                D2D1_COLOR_F color = (interactiveIdx == selecaoAtual) ? D2D1::ColorF(0.2f, 1.0f, 0.2f) : D2D1::ColorF(0.9f, 0.9f, 0.9f);
                box.AddText(text, lw/2.0f, currentY, 16.0f, color, true);
                interactiveIdx++;
                currentY += 30.0f;
            }
        }
    };
    
    auto* d2d = D2DContext::renderer;
    if (d2d) {
        UIDynamicBox box;
        float lw = UIRenderer2D::LOGICAL_WIDTH;
        float lh = UIRenderer2D::LOGICAL_HEIGHT;
        renderCb(box, selecaoAtual, lw, lh);
        box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.95f, 2.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f), 20.0f, lw/2.0f, lh/2.0f);
        outW = 800; // Valor arbitrario para marcar como desenhado
        outH = 600;
    }
}
