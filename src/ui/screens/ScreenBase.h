#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include "../../core/utils/Color.h"
class Character;

class TelaBase {
public:
    static std::string gerarBarraGradiente(double pct, int tamanho, Color corFinal);
    static void imprimirLinhaDivisoria(char caractere = '=');
    static std::vector<std::string> criarCaixa(const std::vector<std::string>& linhas, const std::string& titulo = "", int larguraMinima = 0, Color corCaixa = Color::WHITE, const std::string& bgAnsi = "");
    static std::vector<std::string> criarCaixaComArte(const std::vector<std::string>& arte, const std::vector<std::string>& linhasTexto, const std::string& titulo = "", int larguraMinima = 0, Color corCaixa = Color::WHITE, const std::string& bgAnsi = "");

    static void executarLoop(
        const std::function<void(bool)>& renderCabecalho,
        const std::function<void()>& renderConteudo,
        const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
        const std::function<bool(int)>& processarEscolha,
        bool centralizarMenu = true,
        const std::string& margemMenu = ""
    );
    
    static void executarLoopPadrao(
        const std::string& titulo,
        Color corTema,
        const std::function<void()>& renderConteudo,
        const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
        const std::function<bool(int)>& processarEscolha
    );

    static bool deveAnimarEntradaDaTela(std::chrono::steady_clock::time_point& ultimoAcesso, int delayMilissegundos);
};