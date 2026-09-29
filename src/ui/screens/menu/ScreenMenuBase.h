#pragma once

#include <string>
#include <algorithm>
#include <ostream>
// TelaBaseMenu — utilitarios compartilhados de layout para as telas de menu.
// Centraliza calculos de posicionamento horizontal para evitar repeticao
// em cada tela concreta (Raycaster, IDE, futuras perspectivas).
class TelaBaseMenu {
public:
    // 'comprimentoTexto' caracteres dentro de um terminal de 'larguraConsole' colunas.
    // Nunca retorna valor negativo.
    static int calcularOffsetCentral(int comprimentoTexto, int larguraConsole) {
        return std::max(0, (larguraConsole - comprimentoTexto) / 2);
    }

    // Sobrecarga conveniente: aceita a string diretamente e automaticamente
    // ignora codigos de cor ANSI no calculo da largura visual.
    static int calcularOffsetCentral(const std::string& texto, int larguraConsole) {
        return calcularOffsetCentral((int)(texto).length(), larguraConsole);
    }

    // Desenha uma caixa preta com bordas brancas
    static void desenharCaixaPreta(std::ostream& out, int y, int x, int largura, int altura) {
        std::string bordaTop = "┌";
        for (int i = 0; i < largura - 2; ++i) bordaTop += "─";
        bordaTop += "┐";

        std::string bordaBot = "└";
        for (int i = 0; i < largura - 2; ++i) bordaBot += "─";
        bordaBot += "┘";

        std::string meio = "";
        for (int i = 0; i < largura - 2; ++i) meio += " ";

        // Topo
        out << bordaTop << "\n";
        // Linhas intermediarias
        for (int i = 1; i < altura - 1; ++i) {
            out << "│" << meio << "│\n";
        }
        // Base
        out << bordaBot << "\n";
    }
};
