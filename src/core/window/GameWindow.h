#pragma once

#include <windows.h>
#include <bitset>
#include <atomic>

class GameWindow {
public:
    GameWindow(HINSTANCE hInstance, int nCmdShow);
    ~GameWindow();

    HWND getHWND() const { return m_hwnd; }
    HINSTANCE getHInstance() const { return m_hInstance; }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    bool processMessages();

    static bool isKeyPressed(int vk);
    static void clearKeys();

    static int getMouseX();
    static int getMouseY();
    static bool isMouseClicked();
    static void clearMouse();

    static void hideCursor();
    static void showCursor();
    static bool isCursorHidden();

    // Metodos legados para compatibilidade
    HWND obterHWND() const { return getHWND(); }
    HINSTANCE obterHInstance() const { return getHInstance(); }
    int obterLargura() const { return getWidth(); }
    int obterAltura() const { return getHeight(); }
    bool processarMensagens() { return processMessages(); }
    static bool teclaPressionada(int vk) { return isKeyPressed(vk); }
    static void limparTeclas() { clearKeys(); }
    static int obterMouseX() { return getMouseX(); }
    static int obterMouseY() { return getMouseY(); }
    static bool mouseClicado() { return isMouseClicked(); }
    static void limparMouse() { clearMouse(); }
    static void ocultarCursor() { hideCursor(); }
    static void mostrarCursor() { showCursor(); }
    static bool isCursorOculto() { return isCursorHidden(); }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd = nullptr;
    HINSTANCE m_hInstance = nullptr;
    HICON m_hIcon = nullptr;
    int m_width = 0;
    int m_height = 0;

    static std::bitset<256> s_keys;
    static std::atomic<int> s_mouseX;
    static std::atomic<int> s_mouseY;
    static std::atomic<bool> s_mouseClicked;
    static bool s_cursorHidden;
};

