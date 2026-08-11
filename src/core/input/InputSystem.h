#pragma once

#include <windows.h>

// Sistema de Input Unificado para a Engine Direct2D / Win32
class InputSystem {
public:
    static void Initialize(HWND hwnd);
    static void Update();
    
    static bool IsKeyPressed(int vk);
    static bool WasKeyPressed(int vk);
    static bool IsMouseMoving(int& dx, int& dy);
    
    static int GetMouseX();
    static int GetMouseY();
    static void Clear();
};
