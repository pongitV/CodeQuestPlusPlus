#include "InputSystem.h"
#include "Win32Input.h"

void InputSystem::Initialize(HWND hwnd) {
    Win32Input::initialize(hwnd);
}

void InputSystem::Update() {
    Win32Input::poll();
}

bool InputSystem::IsKeyPressed(int vk) {
    return Win32Input::isKeyPressed(vk);
}

bool InputSystem::WasKeyPressed(int vk) {
    return Win32Input::isKeyJustPressed(vk);
}

bool InputSystem::IsMouseMoving(int& dx, int& dy) {
    return Win32Input::isMouseMoving(dx, dy);
}

int InputSystem::GetMouseX() {
    return Win32Input::getMouseX();
}

int InputSystem::GetMouseY() {
    return Win32Input::getMouseY();
}

void InputSystem::Clear() {
    Win32Input::clear();
}
