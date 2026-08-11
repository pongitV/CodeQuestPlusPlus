#pragma once

#include <string>
#include <vector>
#include "Color.h"
#include <functional>

#include <chrono>

enum class EstadoInput {
    Idle,
    MenuSelecting,
    WaitingEnter,
    DialogReading
};

class InputStateMachine {
public:
    EstadoInput estado = EstadoInput::Idle;
    int menuSelection = 0;
    std::chrono::steady_clock::time_point lastUpdate;

    void update() {
        lastUpdate = std::chrono::steady_clock::now();
    }
};

enum class ComandoMapa {
    Cima,
    Baixo,
    Esquerda,
    Direita,
    Inventory,
    Ficha,
    Bestiary,
    Nenhum
};

class InputControl 
{
public:
    static bool teclaPressionada();
    static char lerTecla();
    static ComandoMapa traduzirTeclaParaComando(char tecla);
    static void limparBuffer();
    static void atualizarTeclas();
    static std::string lerEntradaProtegida(const std::string& promptMensagem = "");
    
    static int lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, bool centralizarPrompt = false, const std::string& margemPersonalizada = "");
    static int lerSelecaoMenuComSetas(const std::vector<std::string>& opcoes, bool centralizar = true, const std::string& margemPersonalizada = "", const std::vector<std::string>& painelDireito = {});
    static int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Color corTema = Color::WHITE, const std::vector<std::string>& arteAscii = {}, bool animarEntrada = true);
    static void aguardarEnter(const std::string& mensagem = "Pressione ENTER para continuar...");
    
    static void executarLoopMenuPopup(
        const std::function<std::vector<std::string>()>& obterDialogo,
        const std::function<std::vector<std::string>()>& obterOpcoes,
        const std::function<bool(const std::string&)>& processarOpcao,
        const std::string& titulo,
        Color corTema,
        const std::vector<std::string>& arteAscii
    );

    // English Aliases
    static bool isKeyPressed() { return teclaPressionada(); }
    static char readKey() { return lerTecla(); }
    static void clearBuffer() { limparBuffer(); }
    static void updateKeys() { atualizarTeclas(); }
    static void waitForEnter(const std::string& msg = "Press ENTER to continue...") { aguardarEnter(msg); }
};
