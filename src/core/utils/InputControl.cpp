#include "InputControl.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <thread>
#include <chrono>

#include "../../rendering/raycaster/screens/utils/MenuD2DUtils.h"
#include "../d2d-context/D2DContext.h"
#include "../window/GameWindow.h"
#include "../../rendering/direct-2d/D2DRenderer.h"
#include "../../core/utils/Color.h"
#include "../../systems/combat/Parry.h"


#include "../input/InputSystem.h"

void InputControl::atualizarTeclas() {
    InputSystem::Update();
}

static bool teclaAgora(int vk) {
    return InputSystem::WasKeyPressed(vk);
}

static void processarMsgED2D() {
    if (auto* win = D2DContext::window) {
        win->processarMensagens();
    }
    InputControl::atualizarTeclas();
    // NOTE: Do NOT call apresentarBackbuffer here — it would overwrite
    // whatever the current screen just drew with the black backbuffer bitmap.
}

bool InputControl::teclaPressionada() {
    for (int vk = 32; vk <= 126; ++vk) {
        if (InputSystem::IsKeyPressed(vk)) return true;
    }
    return InputSystem::IsKeyPressed(VK_RETURN) ||
           InputSystem::IsKeyPressed(VK_ESCAPE) ||
           InputSystem::IsKeyPressed(VK_UP) ||
           InputSystem::IsKeyPressed(VK_DOWN) ||
           InputSystem::IsKeyPressed(VK_BACK);
}

char InputControl::lerTecla() {
    if (InputSystem::WasKeyPressed(VK_RETURN)) return '\r';
    if (InputSystem::WasKeyPressed(VK_ESCAPE)) return 27;
    if (InputSystem::WasKeyPressed(VK_UP)) return 'w';
    if (InputSystem::WasKeyPressed(VK_DOWN)) return 's';
    if (InputSystem::WasKeyPressed(VK_BACK)) return 8;
    for (int vk = 32; vk <= 126; ++vk) {
        if (InputSystem::WasKeyPressed(vk)) return (char)vk;
    }
    return 0;
}

ComandoMapa InputControl::traduzirTeclaParaComando(char tecla) {
    switch (tecla) {
        case 'w': case 'W': return ComandoMapa::Cima;
        case 's': case 'S': return ComandoMapa::Baixo;
        case 'a': case 'A': return ComandoMapa::Esquerda;
        case 'd': case 'D': return ComandoMapa::Direita;
        case 'i': case 'I': return ComandoMapa::Inventory;
        case 'f': case 'F': 
        case 'c': case 'C': return ComandoMapa::Ficha;
        case 'b': case 'B': 
        case 'j': case 'J': return ComandoMapa::Bestiary;
        default: return ComandoMapa::Nenhum;
    }
}

void InputControl::limparBuffer() {
    InputSystem::Clear();
    GameWindow::limparTeclas();
}

std::string InputControl::lerEntradaProtegida(const std::string& promptMensagem) {
    return "";
}

int InputControl::lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, bool centralizarPrompt, const std::string& margemPersonalizada) {
    int valorAtual = minimo;
    limparBuffer();
    
    auto handler = [&](char tecla, int& selecaoAtual) -> bool {
        if (teclaAgora(VK_LEFT) || teclaAgora('A')) {
            if (valorAtual > minimo) valorAtual--;
            return true;
        }
        if (teclaAgora(VK_RIGHT) || teclaAgora('D')) {
            if (valorAtual < maximo) valorAtual++;
            return true;
        }
        return false;
    };
    
    auto construtor = [&](UIDynamicBox& box, int sel, float logicalW, float startCol, float startY) {
        std::wstring prompt = MenuRaycasterUtils::utf8_to_wstring(promptMensagem);
        box.AddText(prompt, logicalW / 2.0f, startY, 18.0f, D2D1::ColorF(D2D1::ColorF::White), true);
        
        std::wstring valorStr = L"<  " + std::to_wstring(valorAtual) + L"  >";
        box.AddText(valorStr, logicalW / 2.0f, startY + 50.0f, 18.0f, D2D1::ColorF(D2D1::ColorF::Yellow), true);
        
        D2D1_COLOR_F corOpcao = (sel == 0) ? D2D1::ColorF(D2D1::ColorF::Cyan) : D2D1::ColorF(0.6f, 0.6f, 0.6f);
        MenuRaycasterUtils::adicionarOpcaoMenu(box, L"CONFIRMAR", logicalW / 2.0f, startY + 100.0f, (sel == 0), corOpcao, true);
    };

    int escolha = MenuRaycasterUtils::renderizarMenuPadrao(
        L"SELECIONAR QUANTIDADE", D2D1::ColorF(0.2f, 0.8f, 0.8f),
        { "CONFIRMAR" }, {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
        construtor, handler
    );
    
    if (escolha == 0) return valorAtual;
    return minimo;
}

int InputControl::lerSelecaoMenuComSetas(const std::vector<std::string>& opcoes, bool centralizar, const std::string& margemPersonalizada, const std::vector<std::string>& painelDireito) {
    processarMsgED2D();
    if (opcoes.empty()) return 0;
    static int ultimaSelecao = 0;
    static auto ultimoUpdate = std::chrono::steady_clock::now();
    auto agora = std::chrono::steady_clock::now();

    if (agora - ultimoUpdate > std::chrono::milliseconds(100)) {
        if (teclaAgora(VK_UP) && ultimaSelecao > 0) {
            ultimaSelecao--;
            ultimoUpdate = agora;
        }
        else if (teclaAgora(VK_DOWN) && ultimaSelecao < (int)opcoes.size() - 1) {
            ultimaSelecao++;
            ultimoUpdate = agora;
        }
    }

    if (teclaAgora(VK_RETURN)) {
        int escolha = ultimaSelecao;
        ultimaSelecao = 0;
        return escolha;
    }
    return -1;
}

int InputControl::lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Color corTema, const std::vector<std::string>& arteAscii, bool animarEntrada) {
    limparBuffer();
    
    D2D1_COLOR_F corD2D = MenuRaycasterUtils::converterCorParaD2D(corTema);
    
    MenuRaycasterUtils::PosicaoArte pos = arteAscii.empty() ? MenuRaycasterUtils::PosicaoArte::NENHUMA : MenuRaycasterUtils::PosicaoArte::ESQUERDA;
    
    std::vector<GrupoCorUI> paletaDinamica = {
        {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-=_+[]{}|;':\",./<>?\\~` ", 
         (int)(corD2D.r * 255), (int)(corD2D.g * 255), (int)(corD2D.b * 255)}
    };

    return MenuRaycasterUtils::renderizarMenuPadrao(
        MenuRaycasterUtils::utf8_to_wstring(titulo),
        corD2D,
        opcoes,
        arteAscii, // Arte fica na esquerda
        paletaDinamica, 
        pos,
        5.0f, // Escala adequada para os NPCs
        nullptr,
        nullptr,
        {}, // Sem titulo gigante
        {},
        texto // Texto renderizado como contexto
    );
}

void InputControl::aguardarEnter(const std::string& /*mensagem*/) {
    limparBuffer();
    GameWindow::limparMouse();

    while (true) {
        processarMsgED2D();

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        }

        if (teclaAgora(VK_RETURN) || teclaAgora(VK_SPACE) || GameWindow::mouseClicado()) {
            limparBuffer();
            GameWindow::limparMouse();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void InputControl::executarLoopMenuPopup(
    const std::function<std::vector<std::string>()>& obterDialogo,
    const std::function<std::vector<std::string>()>& obterOpcoes,
    const std::function<bool(const std::string&)>& processarOpcao,
    const std::string& titulo,
    Color corTema,
    const std::vector<std::string>& arteAscii
) {
    bool emMenu = true;
    while (emMenu) {
        std::vector<std::string> opcoes = obterOpcoes();
        std::vector<std::string> dialogo = obterDialogo();
        
        D2D1_COLOR_F corD2D = MenuRaycasterUtils::converterCorParaD2D(corTema);

        std::vector<GrupoCorUI> paletaDinamica = {
            {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-=_+[]{}|;':\",./<>?\\~` ", 
             (int)(corD2D.r * 255), (int)(corD2D.g * 255), (int)(corD2D.b * 255)}
        };

        auto construtorAdicional = [&](UIDynamicBox& box, int selecaoAtual, float logicalW, float startCol, float startY) {
            float currentY = startY;
            for (const auto& linha : dialogo) {
                box.AddText(MenuRaycasterUtils::utf8_to_wstring(linha), startCol, currentY, 18.0f, D2D1::ColorF(D2D1::ColorF::White), true);
                currentY += 25.0f;
            }
            currentY += 20.0f;
            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::string opcStr = opcoes[i];
                D2D1_COLOR_F corOpcao = (i == selecaoAtual) ? corD2D : D2D1::ColorF(0.6f, 0.6f, 0.6f);
                MenuRaycasterUtils::adicionarOpcaoMenu(box, MenuRaycasterUtils::utf8_to_wstring(opcStr), startCol, currentY, (i == selecaoAtual), corOpcao, true);
                currentY += 40.0f;
            }
        };

        int escolha = MenuRaycasterUtils::renderizarMenuPadrao(
            MenuRaycasterUtils::utf8_to_wstring(titulo),
            corD2D,
            opcoes,
            arteAscii, // arte (esquerda)
            paletaDinamica, // paletaArte
            MenuRaycasterUtils::PosicaoArte::ESQUERDA,
            5.0f, // escalaArte
            construtorAdicional,
            nullptr, // handler
            {}, // tituloAscii
            {}, // paletaAscii
            {} // textoContexto
        );

        if (escolha >= 0 && escolha < (int)opcoes.size()) {
            std::string opcStr = opcoes[escolha];
            std::string opcUpper = opcStr;
            std::transform(opcUpper.begin(), opcUpper.end(), opcUpper.begin(), ::toupper);
            if (opcUpper == "VOLTAR" || opcUpper == "SAIR" || opcUpper == "IR EMBORA" || opcUpper == "CANCELAR") {
                emMenu = false;
            } else {
                emMenu = processarOpcao(opcStr);
            }
        } else {
            emMenu = false;
        }
    }
}
