#pragma once

#include <windows.h>
#include <array>

class Win32Input {
public:
    static void initialize(HWND hwnd);
    static void poll();

    static bool isKeyPressed(int vkCode);
    static bool isKeyJustPressed(int vkCode);
    static bool isMouseMoving(int& deltaX, int& deltaY);
    
    static int getMouseX();
    static int getMouseY();

    static void clear();
};
