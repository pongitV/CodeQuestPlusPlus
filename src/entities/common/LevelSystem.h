#pragma once

// LevelSystem handles character leveling, XP accumulation, and level threshold calculations.
class SistemaDeNivel {
private:
    int level;
    int currentXp;
    int xpToLevelUp;

public:
    SistemaDeNivel(int initialLevel = 1, int initialXp = 0, int initialXpToLevelUp = 100) 
        : level(initialLevel), currentXp(initialXp), xpToLevelUp(initialXpToLevelUp) {}

    int getLevel() const { return level; }
    int getCurrentXp() const { return currentXp; }
    int getXpToLevelUp() const { return xpToLevelUp; }

    void setLevel(int newLevel) { level = newLevel; }
    void setCurrentXp(int newXp) { currentXp = newXp; }
    void setXpToLevelUp(int newXpToLevelUp) { xpToLevelUp = newXpToLevelUp; }

    void addXp(int amount) { currentXp += amount; }
    bool canLevelUp() const { return currentXp >= xpToLevelUp; }

    // Legacy method delegates
    int getXpAtual() const { return getCurrentXp(); }
    int getXpParaSubir() const { return getXpToLevelUp(); }
    void definirNivel(int l) { setLevel(l); }
    void definirXpAtual(int xp) { setCurrentXp(xp); }
    void definirXpParaSubir(int xp) { setXpToLevelUp(xp); }
    void ganharXp(int val) { addXp(val); }
    bool podeSubirDeNivel() const { return canLevelUp(); }
};

using LevelSystem = SistemaDeNivel;
