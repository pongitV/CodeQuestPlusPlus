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

void InputControl::updateKeys() {
    InputSystem::Update();
}

static bool isKeyJustPressed(int vk) {
    return InputSystem::WasKeyPressed(vk);
}

static void processMessagesAndD2D() {
    if (auto* win = D2DContext::window) {
        win->processMessages();
    }
    InputControl::updateKeys();
    // NOTA: NAO chamar apresentarBackbuffer aqui — sobrescreveria
    // o que a tela atual acabou de desenhar com o bitmap preto do backbuffer.
}

bool InputControl::isKeyPressed() {
    for (int vk = 32; vk <= 126; ++vk) {
        if (InputSystem::IsKeyPressed(vk)) return true;
    }
    return InputSystem::IsKeyPressed(VK_RETURN) ||
           InputSystem::IsKeyPressed(VK_ESCAPE) ||
           InputSystem::IsKeyPressed(VK_UP) ||
           InputSystem::IsKeyPressed(VK_DOWN) ||
           InputSystem::IsKeyPressed(VK_BACK);
}

char InputControl::readKey() {
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

MapCommand InputControl::translateKeyToCommand(char key) {
    switch (key) {
        case 'w': case 'W': return MapCommand::Up;
        case 's': case 'S': return MapCommand::Down;
        case 'a': case 'A': return MapCommand::Left;
        case 'd': case 'D': return MapCommand::Right;
        case 'i': case 'I': return MapCommand::Inventory;
        case 'f': case 'F': 
        case 'c': case 'C': return MapCommand::Sheet;
        case 'b': case 'B': 
        case 'j': case 'J': return MapCommand::Bestiary;
        default: return MapCommand::None;
    }
}

void InputControl::clearBuffer() {
    InputSystem::Clear();
    GameWindow::clearKeys();
}

std::string InputControl::readProtectedInput(const std::string& promptMessage) {
    return "";
}

int InputControl::readIntegerWithBounds(const std::string& promptMessage, int minimum, int maximum, bool centerPrompt, const std::string& customMargin) {
    int valorAtual = minimum;
    clearBuffer();
    
    auto handler = [&](char tecla, int& selecaoAtual) -> bool {
        if (isKeyJustPressed(VK_LEFT) || isKeyJustPressed('A')) {
            if (valorAtual > minimum) valorAtual--;
            return true;
        }
        if (isKeyJustPressed(VK_RIGHT) || isKeyJustPressed('D')) {
            if (valorAtual < maximum) valorAtual++;
            return true;
        }
        return false;
    };
    
    auto construtor = [&](UIDynamicBox& box, int sel, float logicalW, float startCol, float startY) {
        std::wstring prompt = MenuRaycasterUtils::utf8_to_wstring(promptMessage);
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
    return minimum;
}

int InputControl::readMenuSelectionWithArrows(const std::vector<std::string>& options, bool center, const std::string& customMargin, const std::vector<std::string>& rightPanel) {
    processMessagesAndD2D();
    if (options.empty()) return 0;
    static int ultimaSelecao = 0;
    static auto ultimoUpdate = std::chrono::steady_clock::now();
    auto agora = std::chrono::steady_clock::now();

    if (agora - ultimoUpdate > std::chrono::milliseconds(100)) {
        if (isKeyJustPressed(VK_UP) && ultimaSelecao > 0) {
            ultimaSelecao--;
            ultimoUpdate = agora;
        }
        else if (isKeyJustPressed(VK_DOWN) && ultimaSelecao < (int)options.size() - 1) {
            ultimaSelecao++;
            ultimoUpdate = agora;
        }
    }

    if (isKeyJustPressed(VK_RETURN)) {
        int escolha = ultimaSelecao;
        ultimaSelecao = 0;
        return escolha;
    }
    return -1;
}

int InputControl::readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& text, const std::vector<std::string>& options, Color themeColor, const std::vector<std::string>& asciiArt, bool animateEntrance) {
    clearBuffer();
    
    D2D1_COLOR_F corD2D = MenuRaycasterUtils::converterCorParaD2D(themeColor);
    
    MenuRaycasterUtils::PosicaoArte pos = asciiArt.empty() ? MenuRaycasterUtils::PosicaoArte::NENHUMA : MenuRaycasterUtils::PosicaoArte::ESQUERDA;
    
    std::vector<GrupoCorUI> paletaDinamica = {
        {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-=_+[]{}|;':\",./<>?\\~` ", 
         (int)(corD2D.r * 255), (int)(corD2D.g * 255), (int)(corD2D.b * 255)}
    };

    return MenuRaycasterUtils::renderizarMenuPadrao(
        MenuRaycasterUtils::utf8_to_wstring(title),
        corD2D,
        options,
        asciiArt, // Arte fica na esquerda
        paletaDinamica, 
        pos,
        5.0f, // Escala adequada para os NPCs
        nullptr,
        nullptr,
        {}, // Sem titulo gigante
        {},
        text // Texto renderizado como contexto
    );
}

void InputControl::waitForEnter(const std::string& /*message*/) {
    clearBuffer();
    GameWindow::clearMouse();

    while (true) {
        processMessagesAndD2D();

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        }

        if (isKeyJustPressed(VK_RETURN) || isKeyJustPressed(VK_SPACE) || GameWindow::isMouseClicked()) {
            clearBuffer();
            GameWindow::clearMouse();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void InputControl::executePopupMenuLoop(
    const std::function<std::vector<std::string>()>& getDialog,
    const std::function<std::vector<std::string>()>& getOptions,
    const std::function<bool(const std::string&)>& processOption,
    const std::string& title,
    Color themeColor,
    const std::vector<std::string>& asciiArt
) {
    bool emMenu = true;
    while (emMenu) {
        std::vector<std::string> opcoes = getOptions();
        std::vector<std::string> dialogo = getDialog();
        
        D2D1_COLOR_F corD2D = MenuRaycasterUtils::converterCorParaD2D(themeColor);

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
            MenuRaycasterUtils::utf8_to_wstring(title),
            corD2D,
            opcoes,
            asciiArt, // arte (esquerda)
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
                emMenu = processOption(opcStr);
            }
        } else {
            emMenu = false;
        }
    }
}
