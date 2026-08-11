#include "Win32Input.h"
#include <mutex>

namespace {
    static std::array<bool, 256> keyStates = {};
    static std::array<bool, 256> previousKeyStates = {};
    static int mouseX = 0;
    static int mouseY = 0;
    static bool firstCapture = true;
    static bool mouseMovementActive = false;
    static HWND initializedWindow = nullptr;
}

void Win32Input::initialize(HWND hwnd) {
    initializedWindow = hwnd;
    for (int i = 0; i < 256; i++) {
        bool down = (GetAsyncKeyState(i) & 0x8000) != 0;
        keyStates[i] = down;
        previousKeyStates[i] = down;
    }
    mouseMovementActive = false;
    firstCapture = true;
}

void Win32Input::poll() {
    previousKeyStates = keyStates;
    
    for (int i = 0; i < 256; i++) {
        keyStates[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

bool Win32Input::isKeyPressed(int vkCode) {
    if (vkCode < 0 || vkCode >= 256) return false;
    return keyStates[vkCode];
}

bool Win32Input::isKeyJustPressed(int vkCode) {
    if (vkCode < 0 || vkCode >= 256) return false;
    return keyStates[vkCode] && !previousKeyStates[vkCode];
}

bool Win32Input::isMouseMoving(int& deltaX, int& deltaY) {
    POINT p;
    HWND hwnd = initializedWindow ? initializedWindow : GetActiveWindow();
    if (hwnd && GetForegroundWindow() == hwnd && GetCursorPos(&p)) {
        RECT rect;
        GetClientRect(hwnd, &rect);
        POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
        ClientToScreen(hwnd, &center);

        if (firstCapture) {
            SetCursorPos(center.x, center.y);
            mouseX = center.x;
            mouseY = center.y;
            firstCapture = false;
            deltaX = 0;
            deltaY = 0;
            return false;
        }

        deltaX = p.x - center.x;
        deltaY = p.y - center.y;

        if (deltaX != 0 || deltaY != 0) {
            SetCursorPos(center.x, center.y);
            mouseX = center.x;
            mouseY = center.y;
            return true;
        }
    }
    deltaX = 0;
    deltaY = 0;
    return false;
}

int Win32Input::getMouseX() { return mouseX; }
int Win32Input::getMouseY() { return mouseY; }

void Win32Input::clear() {
    for (int i = 0; i < 256; i++) {
        bool down = (GetAsyncKeyState(i) & 0x8000) != 0;
        keyStates[i] = down;
        previousKeyStates[i] = down;
    }
    mouseMovementActive = false;
    firstCapture = true;
}
