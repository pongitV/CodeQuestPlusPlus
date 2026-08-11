#pragma once

#include <windows.h>
#include <bitset>
#include <atomic>

class GameWindow {
public:
    GameWindow(HINSTANCE hInstance, int nCmdShow);
    ~GameWindow();

    HWND obterHWND() const { return m_hwnd; }
    HINSTANCE obterHInstance() const { return m_hInstance; }

    int obterLargura() const { return m_largura; }
    int obterAltura() const { return m_altura; }

    bool processarMensagens();

    static bool teclaPressionada(int vk);
    static void limparTeclas();

    static int obterMouseX();
    static int obterMouseY();
    static bool mouseClicado();
    static void limparMouse();

    // English Aliases
    HWND getHWND() const { return obterHWND(); }
    HINSTANCE getHInstance() const { return obterHInstance(); }
    int getWidth() const { return obterLargura(); }
    int getHeight() const { return obterAltura(); }
    bool processMessages() { return processarMensagens(); }
    static bool isKeyPressed(int vk) { return teclaPressionada(vk); }
    static int getMouseX() { return obterMouseX(); }
    static int getMouseY() { return obterMouseY(); }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd = nullptr;
    HINSTANCE m_hInstance = nullptr;
    HICON m_hIcon = nullptr;
    int m_largura = 0;
    int m_altura = 0;

    static std::bitset<256> s_teclas;
    static std::atomic<int> s_mouseX;
    static std::atomic<int> s_mouseY;
    static std::atomic<bool> s_mouseClicado;
};

