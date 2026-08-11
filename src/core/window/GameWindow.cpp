#include "GameWindow.h"
#include "../d2d-context/D2DContext.h"
#include "../../rendering/direct-2d/D2DRenderer.h"
#include <string>
#include <cstring>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

std::bitset<256> GameWindow::s_teclas;
std::atomic<int> GameWindow::s_mouseX{0};
std::atomic<int> GameWindow::s_mouseY{0};
std::atomic<bool> GameWindow::s_mouseClicado{false};

LRESULT CALLBACK GameWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            if (wParam < 256) s_teclas.set(wParam);
            return 0;
        case WM_KEYUP:
            if (wParam < 256) s_teclas.reset(wParam);
            return 0;
        case WM_MOUSEMOVE:
            s_mouseX = (int)(short)LOWORD(lParam);
            s_mouseY = (int)(short)HIWORD(lParam);
            return 0;
        case WM_LBUTTONDOWN:
            s_mouseX = (int)(short)LOWORD(lParam);
            s_mouseY = (int)(short)HIWORD(lParam);
            s_mouseClicado = true;
            return 0;
        case WM_SIZE:
            if (D2DContext::renderer) {
                D2DContext::renderer->redimensionar(hwnd);
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            (void)hdc;
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                ClipCursor(nullptr);
                ShowCursor(TRUE);
            }
            return 0;
        case WM_KILLFOCUS:
            ClipCursor(nullptr);
            ShowCursor(TRUE);
            return 0;
        case WM_DESTROY:
            ClipCursor(nullptr);
            ShowCursor(TRUE);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static std::wstring resolverCaminhoAsset(const wchar_t* meuarquivo) {
    if (GetFileAttributesW(meuarquivo) != INVALID_FILE_ATTRIBUTES) {
        return meuarquivo;
    }
    std::wstring relParent = std::wstring(L"../") + meuarquivo;
    if (GetFileAttributesW(relParent.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return relParent;
    }
    constexpr DWORD maxPathLen = 2048;
    wchar_t exePath[maxPathLen];
    if (GetModuleFileNameW(nullptr, exePath, maxPathLen)) {
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (!lastSlash) lastSlash = wcsrchr(exePath, L'/');
        if (lastSlash) {
            *lastSlash = L'\0';
            std::wstring absPath = std::wstring(exePath) + L"/" + meuarquivo;
            if (GetFileAttributesW(absPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                return absPath;
            }
            wchar_t* parentSlash = wcsrchr(exePath, L'\\');
            if (!parentSlash) parentSlash = wcsrchr(exePath, L'/');
            if (parentSlash) {
                *parentSlash = L'\0';
                absPath = std::wstring(exePath) + L"/" + meuarquivo;
                if (GetFileAttributesW(absPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    return absPath;
                }
            }
        }
    }
    return meuarquivo;
}

static HICON carregarIconeDePNG(const wchar_t* meuarquivo) {
    std::wstring caminhoFinal = resolverCaminhoAsset(meuarquivo);

    ULONG_PTR token = 0;
    Gdiplus::GdiplusStartupInput input;
    if (Gdiplus::GdiplusStartup(&token, &input, nullptr) != Gdiplus::Ok) {
        return nullptr;
    }

    HICON hIcon = nullptr;
    {
        Gdiplus::Bitmap bitmap(caminhoFinal.c_str());
        if (bitmap.GetLastStatus() == Gdiplus::Ok) {
            bitmap.GetHICON(&hIcon);
        }
    }

    Gdiplus::GdiplusShutdown(token);
    return hIcon;
}

GameWindow::GameWindow(HINSTANCE hInstance, int nCmdShow)
    : m_hInstance(hInstance)
{
    const char CLASS_NAME[] = "CodeQuestPlusPlus_Window";

    m_hIcon = carregarIconeDePNG(L"assets/icons/icon.png");
    if (!m_hIcon) {
        m_hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    }
    if (!m_hIcon) {
        m_hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    }

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hIcon = m_hIcon;
    wc.hIconSm = m_hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = CLASS_NAME;

    RegisterClassEx(&wc);

    m_largura = GetSystemMetrics(SM_CXSCREEN);
    m_altura = GetSystemMetrics(SM_CYSCREEN);
    
    int posX = 0;
    int posY = 0;

    m_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "CodeQuestPlusPlus",
        WS_POPUP,
        posX, posY, m_largura, m_altura,
        nullptr, nullptr, hInstance, nullptr
    );

    if (m_hwnd) {
        if (m_hIcon) {
            SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
            SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
        }
        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);
    }
}

GameWindow::~GameWindow() {
    ClipCursor(nullptr);
    ShowCursor(TRUE);
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    if (m_hIcon) {
        DestroyIcon(m_hIcon);
        m_hIcon = nullptr;
    }
}

bool GameWindow::processarMensagens() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

#include "../input/InputSystem.h"

bool GameWindow::teclaPressionada(int vk) {
    if (vk < 0 || vk >= 256) return false;
    return s_teclas.test(vk) || InputSystem::IsKeyPressed(vk);
}

void GameWindow::limparTeclas() {
    s_teclas.reset();
}

int GameWindow::obterMouseX() { return s_mouseX; }
int GameWindow::obterMouseY() { return s_mouseY; }
bool GameWindow::mouseClicado() { return s_mouseClicado; }
void GameWindow::limparMouse() { s_mouseClicado = false; }

