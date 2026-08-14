#pragma once

#include <atomic>

class Character;

class Debug {
public:
    static std::atomic<bool> isGodModeActive;
    static std::atomic<bool> isNoclipActive;
    static std::atomic<bool> isOneHitKillActive;
    static std::atomic<bool> isSpeedHackActive;

    static void showDebugMenu(Character* player);
    static void displayDebugMenu(Character* player) { showDebugMenu(player); }
    static bool isDebugKey(char key = 0);
};
