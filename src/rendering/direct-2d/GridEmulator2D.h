#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "D2DRenderer.h"

class GridEmulator2D {
public:
    GridEmulator2D(D2DRenderer& renderer);

    void renderizarGrid(const std::vector<std::string>& grid, int offsetX = 0, int offsetY = 0);
    void renderizarGridRaw(const std::vector<std::string>& grid, int offsetX = 0, int offsetY = 0);

    void definirCelulaTamanho(float px) { m_celulaTamanho = px; }
    void definirGridDimensoes(int cols, int rows);

    int obterColunas() const { return m_colunas; }
    int obterLinhas() const { return m_linhas; }
    float obterCelulaTamanho() const { return m_celulaTamanho; }

    float obterLarguraTotal() const { return m_colunas * m_celulaTamanho; }
    float obterAlturaTotal() const { return m_linhas * m_celulaTamanho; }

    // English Aliases
    void setCellSize(float px) { definirCelulaTamanho(px); }
    void setGridDimensions(int cols, int rows) { definirGridDimensoes(cols, rows); }
    int getColumns() const { return obterColunas(); }
    int getRows() const { return obterLinhas(); }
    float getCellSize() const { return obterCelulaTamanho(); }
    float getTotalWidth() const { return obterLarguraTotal(); }
    float getTotalHeight() const { return obterAlturaTotal(); }
    void renderGrid(const std::vector<std::string>& grid, int offsetX = 0, int offsetY = 0) { renderizarGrid(grid, offsetX, offsetY); }

private:
    D2DRenderer& m_renderer;
    float m_celulaTamanho = 12.0f;
    int m_colunas = 80;
    int m_linhas = 30;
};
