#pragma once

#include <string>

#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include <vector>
#include <functional>

#include "../../../../core/utils/Color.h"

namespace MenuRaycasterUtils {

    extern float s_tempoMenuAnimacao;
    extern bool s_mostrarLogoAbertura;
    
    // Renders sky and map grid. Pass abrirFrame=true to open a BeginDraw before drawing.
    void desenharFundoNativoD2D(bool abrirFrame = false);

    // General purpose box popup loop
    enum class PosicaoArte { NENHUMA, ACIMA, ABAIXO, ESQUERDA, DIREITA };

    int renderizarMenuPadrao(
        const std::wstring& titulo,
        D2D1_COLOR_F corTema,
        const std::vector<std::string>& opcoes,
        const std::vector<std::string>& arte = {},
        const std::vector<GrupoCorUI>& paletaArte = {},
        PosicaoArte posicaoArte = PosicaoArte::NENHUMA,
        float escalaArte = 8.0f,
        std::function<void(UIDynamicBox&, int, float, float, float)> construtorAdicional = nullptr,
        std::function<bool(char, int&)> handlerEntradaExtra = nullptr,
        const std::vector<std::string>& tituloAscii = {},
        const std::vector<GrupoCorUI>& paletaAscii = {},
        const std::vector<std::string>& textoContexto = {}
    );

    int renderizarPopupCaixa(
        const std::vector<std::string>& tituloAscii,
        const std::vector<GrupoCorUI>& paletaAscii,
        int numOpcoes,
        std::function<void(UIDynamicBox&, int, float, float)> construtorCaixa,
        std::function<bool(char, int&)> handlerEntradaExtra = nullptr
    );

    std::wstring utf8_to_wstring(const std::string& str);
    inline std::string stripAnsi(const std::string& str) { return str; }
    size_t obterComprimentoVisivel(const std::string& str);
    D2D1_COLOR_F converterCorParaD2D(Color cor);
    std::wstring toWStringClean(const std::string& str);
    std::string lerEntradaTextoD2D(const std::wstring& prompt, int maxLength = 10);
    
    void adicionarOpcaoMenu(UIDynamicBox& box, const std::wstring& texto, float x, float y, bool selecionado, D2D1_COLOR_F corBase = D2D1::ColorF(0.6f, 0.6f, 0.6f), bool centralizado = false);

}

