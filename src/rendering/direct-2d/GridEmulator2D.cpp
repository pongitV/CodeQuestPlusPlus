#include "GridEmulator2D.h"
#include <sstream>
#include <cctype>

GridEmulator2D::GridEmulator2D(D2DRenderer& renderer)
    : m_renderer(renderer)
{
}

void GridEmulator2D::definirGridDimensoes(int cols, int rows) {
    m_colunas = cols;
    m_linhas = rows;
}

void GridEmulator2D::renderizarGrid(const std::vector<std::string>& grid, int offsetX, int offsetY) {
    auto* rt = m_renderer.obterRenderTarget();
    auto* fmt = m_renderer.obterFontePadrao();
    if (!rt || grid.empty()) return;

    D2D1_SIZE_F tam = rt->GetSize();
    float cellH_fitW = (tam.width / (float)m_colunas) / 0.55f; 
    float cellH_fitH = tam.height / (float)m_linhas;
    
    float cellH = std::min(cellH_fitW, cellH_fitH);
    float cellW = cellH * 0.55f;
    float fontSize = cellH * 0.95f;
    if (fontSize < 8.0f) fontSize = 8.0f;

    float realOffsetX = offsetX + (tam.width - (cellW * m_colunas)) / 2.0f;
    float realOffsetY = offsetY + (tam.height - (cellH * m_linhas)) / 2.0f;

    D2D1_COLOR_F fgPadrao = D2D1::ColorF(0.8f, 0.8f, 0.8f);

    int linhasRender = std::min((int)grid.size(), m_linhas);

    for (int y = 0; y < linhasRender; y++) {
        const std::string& linha = grid[y];
        int colsRender = std::min((int)linha.size(), m_colunas);

        for (int x = 0; x < colsRender; x++) {
            unsigned char ch = (unsigned char)linha[x];

            if (ch == 0) continue;

            float px = realOffsetX + (float)x * cellW;
            float py = realOffsetY + (float)y * cellH;

            if (ch == ' ') {
                continue;
            }

            if (ch >= 32 && ch <= 126) {
                wchar_t wch = (wchar_t)ch;
                std::wstring str(1, wch);
                m_renderer.desenharTexto(str, px, py, fgPadrao, cellH * 0.8f);
            }
        }
    }
}

void GridEmulator2D::renderizarGridRaw(const std::vector<std::string>& grid, int offsetX, int offsetY) {
    auto* fmt = m_renderer.obterFontePadrao();
    auto* rt = m_renderer.obterRenderTarget();
    if (!fmt || !rt || grid.empty()) return;

    D2D1_SIZE_F tam = rt->GetSize();
    float cellH_fitW = (tam.width / (float)m_colunas) / 0.55f; 
    float cellH_fitH = tam.height / (float)m_linhas;
    
    float cellH = std::min(cellH_fitW, cellH_fitH);
    float cellW = cellH * 0.55f;
    float fontSize = cellH * 0.95f;
    if (fontSize < 8.0f) fontSize = 8.0f;

    float realOffsetX = offsetX + (tam.width - (cellW * m_colunas)) / 2.0f;
    float realOffsetY = offsetY + (tam.height - (cellH * m_linhas)) / 2.0f;

    for (int y = 0; y < (int)grid.size() && y < m_linhas; y++) {
        const std::string& linha = grid[y];
        std::wstring wline(linha.begin(), linha.end());
        float py = realOffsetY + (float)y * cellH;
        m_renderer.desenharTexto(wline, realOffsetX, py, D2D1::ColorF(1, 1, 1), fontSize);
    }
}
