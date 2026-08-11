#pragma once

#include <atomic>

class Character;

class Debug {
public:
    static std::atomic<bool> isGodModeActive;
    static std::atomic<bool> isNoclipActive;
    static std::atomic<bool> isOneHitKillActive;
    static std::atomic<bool> isSpeedHackActive;

    static void displayDebugMenu(Character* jogador);
    static void showDebugMenu(Character* player) { displayDebugMenu(player); }
    static bool isDebugKey(char tecla = 0);
};
