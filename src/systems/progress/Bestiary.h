#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <mutex>

struct BestiaryEnemyInfo {
    std::string name;
    std::string map;
    std::string habitat;
    std::vector<std::string> appearance;
    std::string lore;
    std::string funFact;
    std::vector<std::string> attributesText;
    std::vector<std::string> activeAbilities;
    std::string passiveAbility;
    std::vector<std::string> drops;
    int difficulty = 0; // Dificuldade base para ordenar no menu
};

using SistemaBestiarioEnemyInfo = BestiaryEnemyInfo;

class Bestiary {
public:
    static Bestiary& instance();
    static Bestiary& instancia() { return instance(); }

    void initializeEnemies();
    void initializeInimigos() { initializeEnemies(); }

    void registerFirstSight(const std::string& enemyName);
    void registerDefeat(const std::string& enemyName);
    void registerAbilitySeen(const std::string& enemyName, const std::string& ability);
    void registerDrop(const std::string& enemyName, const std::string& drop);

    bool isDiscovered(const std::string& enemyName) const;
    bool isDefeated(const std::string& enemyName) const;
    int getDefeatCount(const std::string& enemyName) const;
    bool hasSeenAbility(const std::string& enemyName, const std::string& ability) const;
    bool hasCollectedDrop(const std::string& enemyName, const std::string& drop) const;

    const BestiaryEnemyInfo* getInfo(const std::string& enemyName) const;
    std::vector<std::string> getEnemiesOrderedByDifficulty() const;

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

    // Aliases legados em portugues
    void registrarPrimeiraVista(const std::string& nomeInimigo) { registerFirstSight(nomeInimigo); }
    void registrarDerrota(const std::string& nomeInimigo) { registerDefeat(nomeInimigo); }
    void registrarHabilidadeVista(const std::string& nomeInimigo, const std::string& habilidade) { registerAbilitySeen(nomeInimigo, habilidade); }
    void registrarDrop(const std::string& nomeInimigo, const std::string& drop) { registerDrop(nomeInimigo, drop); }

    bool estaDescoberto(const std::string& nomeInimigo) const { return isDiscovered(nomeInimigo); }
    bool jaDerrotado(const std::string& nomeInimigo) const { return isDefeated(nomeInimigo); }
    int obterQuantidadeDerrotas(const std::string& nomeInimigo) const { return getDefeatCount(nomeInimigo); }
    bool jaViuHabilidade(const std::string& nomeInimigo, const std::string& habilidade) const { return hasSeenAbility(nomeInimigo, habilidade); }
    bool jaColetouDrop(const std::string& nomeInimigo, const std::string& drop) const { return hasCollectedDrop(nomeInimigo, drop); }

    const BestiaryEnemyInfo* obterInfo(const std::string& nomeInimigo) const { return getInfo(nomeInimigo); }
    std::vector<std::string> obterInimigosOrdenadosPorDificuldade() const { return getEnemiesOrderedByDifficulty(); }

    void salvar(std::ofstream& out) const { save(out); }
    void carregar(std::ifstream& in) { load(in); }

private:
    Bestiary();
    std::map<std::string, BestiaryEnemyInfo> baseEnemies;
    
    std::set<std::string> seenEnemies;
    std::set<std::string> defeatedEnemies;
    std::map<std::string, int> defeatCountMap;
    std::map<std::string, std::set<std::string>> seenAbilitiesMap;
    std::map<std::string, std::set<std::string>> collectedDropsMap;
    
    mutable std::mutex mtx;
};
