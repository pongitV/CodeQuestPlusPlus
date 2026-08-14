#pragma once

#include <string>
#include <vector>
#include "Color.h"
#include <functional>
#include <chrono>

enum class InputState {
    Idle,
    MenuSelecting,
    WaitingEnter,
    DialogReading
};

using EstadoInput = InputState;

class InputStateMachine {
public:
    InputState state = InputState::Idle;
    int menuSelection = 0;
    std::chrono::steady_clock::time_point lastUpdate;

    void update() {
        lastUpdate = std::chrono::steady_clock::now();
    }
};

enum class MapCommand {
    Up,
    Down,
    Left,
    Right,
    Inventory,
    Sheet,
    CharacterSheet = Sheet,
    Bestiary,
    None,

    // Compatibilidade legada
    Cima = Up,
    Baixo = Down,
    Esquerda = Left,
    Direita = Right,
    Ficha = Sheet,
    Nenhum = None
};

using ComandoMapa = MapCommand;

class InputControl 
{
public:
    static bool isKeyPressed();
    static char readKey();
    static MapCommand translateKeyToCommand(char key);
    static void clearBuffer();
    static void updateKeys();
    static std::string readProtectedInput(const std::string& promptMessage = "");
    
    static int readIntegerWithBounds(const std::string& promptMessage, int minimum, int maximum, bool centerPrompt = false, const std::string& customMargin = "");
    static int readMenuSelectionWithArrows(const std::vector<std::string>& options, bool center = true, const std::string& customMargin = "", const std::vector<std::string>& rightPanel = {});
    static int readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& text, const std::vector<std::string>& options, Color themeColor = Color::WHITE, const std::vector<std::string>& asciiArt = {}, bool animateEntrance = true);
    static void waitForEnter(const std::string& message = "Pressione ENTER para continuar...");
    
    static void executePopupMenuLoop(
        const std::function<std::vector<std::string>()>& getDialog,
        const std::function<std::vector<std::string>()>& getOptions,
        const std::function<bool(const std::string&)>& processOption,
        const std::string& title,
        Color themeColor,
        const std::vector<std::string>& asciiArt
    );

    // Métodos legados para compatibilidade retroativa
    static bool teclaPressionada() { return isKeyPressed(); }
    static char lerTecla() { return readKey(); }
    static MapCommand traduzirTeclaParaComando(char tecla) { return translateKeyToCommand(tecla); }
    static void limparBuffer() { clearBuffer(); }
    static void atualizarTeclas() { updateKeys(); }
    static std::string lerEntradaProtegida(const std::string& promptMensagem = "") { return readProtectedInput(promptMensagem); }
    static int lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, bool centralizarPrompt = false, const std::string& margemPersonalizada = "") {
        return readIntegerWithBounds(promptMensagem, minimo, maximo, centralizarPrompt, margemPersonalizada);
    }
    static int lerSelecaoMenuComSetas(const std::vector<std::string>& opcoes, bool centralizar = true, const std::string& margemPersonalizada = "", const std::vector<std::string>& painelDireito = {}) {
        return readMenuSelectionWithArrows(opcoes, centralizar, margemPersonalizada, painelDireito);
    }
    static int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Color corTema = Color::WHITE, const std::vector<std::string>& arteAscii = {}, bool animarEntrada = true) {
        return readMenuSelectionInPopup(titulo, texto, opcoes, corTema, arteAscii, animarEntrada);
    }
    static void aguardarEnter(const std::string& mensagem = "Pressione ENTER para continuar...") { waitForEnter(mensagem); }
    static void executarLoopMenuPopup(
        const std::function<std::vector<std::string>()>& obterDialogo,
        const std::function<std::vector<std::string>()>& obterOpcoes,
        const std::function<bool(const std::string&)>& processarOpcao,
        const std::string& titulo,
        Color corTema,
        const std::vector<std::string>& arteAscii
    ) {
        executePopupMenuLoop(obterDialogo, obterOpcoes, processarOpcao, titulo, corTema, arteAscii);
    }
};
