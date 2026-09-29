#pragma once

// LevelSystem gerencia o nivel do personagem, acumulo de XP e calculo de progressao.
class LevelSystem {
private:
    int level;
    int currentXp;
    int xpToLevelUp;

public:
    LevelSystem(int initialLevel = 1, int initialXp = 0, int initialXpToLevelUp = 100) 
        : level(initialLevel), currentXp(initialXp), xpToLevelUp(initialXpToLevelUp) {}

    int getLevel() const { return level; }
    int getCurrentXp() const { return currentXp; }
    int getXpToLevelUp() const { return xpToLevelUp; }

    void setLevel(int newLevel) { level = newLevel; }
    void setCurrentXp(int newXp) { currentXp = newXp; }
    void setXpToLevelUp(int newXpToLevelUp) { xpToLevelUp = newXpToLevelUp; }

    void addXp(int amount) { currentXp += amount; }
    bool canLevelUp() const { return currentXp >= xpToLevelUp; }

    // Metodos legados para compatibilidade retroativa
    int getXpAtual() const { return getCurrentXp(); }
    int getXpParaSubir() const { return getXpToLevelUp(); }
    void definirNivel(int l) { setLevel(l); }
    void definirXpAtual(int xp) { setCurrentXp(xp); }
    void definirXpParaSubir(int xp) { setXpToLevelUp(xp); }
    void ganharXp(int val) { addXp(val); }
    bool podeSubirDeNivel() const { return canLevelUp(); }
};

using SistemaDeNivel = LevelSystem;
