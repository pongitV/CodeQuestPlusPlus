#include "ScreenBase.h"
#include "../UIManager.h"
#include "../../core/utils/InputControl.h"
#include "../../core/d2d-context/D2DContext.h"
#include "../../rendering/direct-2d/D2DRenderer.h"
#include "../../core/window/GameWindow.h"
#include "../../entities/character/Character.h"
#include "../../entities/races/RaceBase.h"
#include "../../entities/classes/ClassBase.h"
#include "../../systems/inventory/Inventory.h"
#include <iostream>
#include <algorithm>
#include "../../core/utils/Color.h"

std::string TelaBase::gerarBarraGradiente(double pct, int tamanho, Color corFinal) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;
    int qtdReal = static_cast<int>(pct * tamanho * 8);
    std::string barra;
    barra.reserve(tamanho * 4);
    for (int i = 0; i < tamanho; ++i) {
        int intensidade = 130 + (125 * i) / std::max(1, tamanho - 1);
        std::string corAtual = "";
        int charIdx = i * 8;
        if (qtdReal >= charIdx + 4) barra += corAtual + "█";
        else barra += std::string("░");
    }
    return barra;
}

void TelaBase::imprimirLinhaDivisoria(char caractere) {
    std::string linha = "";
    int largura = 120;
    
    bool isEngineIDE = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    if (isEngineIDE) {
        linha = "// ";
        for (int i = 0; i < largura - 3; ++i) linha += "=";
        return;
    }

    if (caractere == '=') {
        for (int i = 0; i < largura; ++i) linha += "═";
    } else if (caractere == '-') {
        for (int i = 0; i < largura; ++i) linha += "─";
    } else {
        linha = std::string(largura, caractere);
    }
}

void TelaBase::executarLoop(
    const std::function<void(bool)>& renderCabecalho,
    const std::function<void()>& renderConteudo,
    const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
    const std::function<bool(int)>& processarEscolha,
    bool centralizarMenu,
    const std::string& margemMenu)
{
    bool primeiraVez = true;
    while (true) {
        
        if (auto* win = D2DContext::window) {
            if (!win->processarMensagens()) break;
        }

        if (renderCabecalho) {
            renderCabecalho(primeiraVez);
            primeiraVez = false;
        }

        if (renderConteudo) {
            renderConteudo();
        }

        if (auto* d2d = D2DContext::renderer) {
            d2d->comecarQuadro();
            d2d->limpar(D2D1::ColorF(0.1f, 0.12f, 0.15f));
            d2d->finalizarQuadro();
        }

        std::vector<std::string> opcoes = construtorOpcoesMenu();
        int escolha = InputControl::lerSelecaoMenuComSetas(opcoes, centralizarMenu, margemMenu);
        
        if (!processarEscolha(escolha)) {
            break;
        }
    }
}

void TelaBase::executarLoopPadrao(
    const std::string& titulo,
    Color corTema,
    const std::function<void()>& renderConteudo,
    const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
    const std::function<bool(int)>& processarEscolha)
{
    executarLoop(
        [titulo, corTema](bool animar) { ""; },
        renderConteudo,
        construtorOpcoesMenu,
        processarEscolha
    );
}

std::vector<std::string> TelaBase::criarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, int larguraMinima, Color corCaixa, const std::string& bgAnsi) {
    int maxLargura = larguraMinima;
    for (const auto& linha : linhas) {
        int comp = (int)(linha).length();
        if (comp > maxLargura) maxLargura = comp;
    }
    
    std::vector<std::string> caixa;

    bool isEngineIDE = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    if (isEngineIDE) {
        std::string tituloIDE = titulo.empty() ? "Info" : titulo;
        std::replace(tituloIDE.begin(), tituloIDE.end(), ' ', '_');
        
        caixa.push_back("struct " + tituloIDE + " {");
        for (const auto& linha : linhas) {
            std::string linhaLimpa = linha;
            if (linhaLimpa.find(":") != std::string::npos) {
                size_t pos = linhaLimpa.find(":");
                std::string chave = linhaLimpa.substr(0, pos);
                std::string valor = linhaLimpa.substr(pos + 1);
                
                std::string chaveVar = chave;
                chaveVar.erase(std::remove(chaveVar.begin(), chaveVar.end(), ' '), chaveVar.end());
                
                linhaLimpa = "    auto " + chaveVar + " = " + valor + ";";
            } else {
                linhaLimpa = "    " + linha + ";";
            }
            
            int comp = (int)(linhaLimpa).length();
            int padding = maxLargura - comp;
            if (padding > 0) linhaLimpa += std::string(padding, ' ');
            
            caixa.push_back(linhaLimpa);
        }
        caixa.push_back("};");
        return caixa;
    }

    std::string top = "╔";
    int tituloLen = (int)(titulo).length();
    
    if (tituloLen > 0) {
        top += "══ " + titulo + " ";
        int restantes = maxLargura + 2 - (tituloLen + 4);
        if (restantes < 0) restantes = 0;
        for (int i = 0; i < restantes; ++i) {
            top += "═";
        }
    } else {
        for (int i = 0; i < maxLargura + 2; ++i) {
            top += "═";
        }
    }
    top += "╗";
    caixa.push_back(top);

    for (const auto& linha : linhas) {
        int comp = (int)(linha).length();
        int padding = maxLargura - comp;
        caixa.push_back("║ " + linha + std::string(padding > 0 ? padding : 0, ' ') + " ║");
    }

    std::string bottom = "╚";
    for (int i = 0; i < maxLargura + 2; ++i) {
        bottom += "═";
    }
    bottom += "╝";
    caixa.push_back(bottom);

    return caixa;
}

std::vector<std::string> TelaBase::criarCaixaComArte(const std::vector<std::string>& arte, const std::vector<std::string>& linhasTexto, const std::string& titulo, int larguraMinima, Color corCaixa, const std::string& bgAnsi) {
    int larguraArte = 0;
    for (const auto& l : arte) {
        int len = (int)(l).length();
        if (len > larguraArte) larguraArte = len;
    }

    int larguraTexto = larguraMinima;
    for (const auto& l : linhasTexto) {
        int len = (int)(l).length();
        if (len > larguraTexto) larguraTexto = len;
    }

    bool temArte = larguraArte > 0;
    int totalWidth = larguraTexto;
    if (temArte) totalWidth += larguraArte + 3;

    if (totalWidth < larguraMinima) totalWidth = larguraMinima;

    int boxHeight = std::max(static_cast<int>(arte.size()), static_cast<int>(linhasTexto.size()));

    std::vector<std::string> caixa;

    std::string top = "╔";
    int tituloLen = (int)(titulo).length();
    if (tituloLen > 0) {
        top += "══ " + titulo + " ";
        int restantes = totalWidth + 2 - (tituloLen + 4);
        if (restantes < 0) restantes = 0;
        for (int i = 0; i < restantes; ++i) top += "═";
    } else {
        for (int i = 0; i < totalWidth + 2; ++i) top += "═";
    }
    top += "╗";
    caixa.push_back(top);

    for (int i = 0; i < boxHeight; ++i) {
        std::string linhaArte = (i < static_cast<int>(arte.size())) ? arte[i] : "";
        int compArte = (int)(linhaArte).length();
        int padArte = larguraArte - compArte;

        std::string linhaTexto = (i < static_cast<int>(linhasTexto.size())) ? linhasTexto[i] : "";
        int compTexto = (int)(linhaTexto).length();
        int padTexto = larguraTexto - compTexto;

        std::string row;
        if (temArte) {
            row = "║ " + linhaArte + std::string(padArte > 0 ? padArte : 0, ' ') + " ║ " + linhaTexto + std::string(padTexto > 0 ? padTexto : 0, ' ') + " ║";
        } else {
            row = "║ " + linhaTexto + std::string(padTexto > 0 ? padTexto : 0, ' ') + " ║";
        }
        caixa.push_back(row);
    }

    std::string bottom = "╚";
    for (int i = 0; i < totalWidth + 2; ++i) bottom += "═";
    bottom += "╝";
    caixa.push_back(bottom);

    return caixa;
}

bool TelaBase::deveAnimarEntradaDaTela(std::chrono::steady_clock::time_point& ultimoAcesso, int delayMilissegundos) {
    auto agora = std::chrono::steady_clock::now();
    bool animar = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoAcesso).count() > delayMilissegundos;
    ultimoAcesso = agora;
    return animar;
}