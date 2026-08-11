#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <mutex>

struct SistemaBestiarioEnemyInfo {
    std::string nome;
    std::string map;
    std::string habitat;
    std::vector<std::string> appearance;
    std::string lore;
    std::string funFact;
    std::vector<std::string> atributosTexto;
    std::vector<std::string> habilidadesAtivas;
    std::string habilidadePassiva;
    std::vector<std::string> drops;
    int difficulty; // Dificuldade base para ordenar no menu
};

class Bestiary {
public:
    static Bestiary& instancia();
    static Bestiary& instance() { return instancia(); }

    void initializeInimigos();

    void registrarPrimeiraVista(const std::string& nomeInimigo);
    void registrarDerrota(const std::string& nomeInimigo);
    void registrarHabilidadeVista(const std::string& nomeInimigo, const std::string& habilidade);
    void registrarDrop(const std::string& nomeInimigo, const std::string& drop);

    bool estaDescoberto(const std::string& nomeInimigo) const;
    bool jaDerrotado(const std::string& nomeInimigo) const;
    int obterQuantidadeDerrotas(const std::string& nomeInimigo) const;
    bool jaViuHabilidade(const std::string& nomeInimigo, const std::string& habilidade) const;
    bool jaColetouDrop(const std::string& nomeInimigo, const std::string& drop) const;

    const SistemaBestiarioEnemyInfo* obterInfo(const std::string& nomeInimigo) const;
    std::vector<std::string> obterInimigosOrdenadosPorDificuldade() const;

    void salvar(std::ofstream& out) const;
    void carregar(std::ifstream& in);

    // English Aliases
    void registerFirstSight(const std::string& name) { registrarPrimeiraVista(name); }
    void registerDefeat(const std::string& name) { registrarDerrota(name); }
    bool isDiscovered(const std::string& name) const { return estaDescoberto(name); }
    bool isDefeated(const std::string& name) const { return jaDerrotado(name); }
    int getDefeatCount(const std::string& name) const { return obterQuantidadeDerrotas(name); }
    const SistemaBestiarioEnemyInfo* getInfo(const std::string& name) const { return obterInfo(name); }
    std::vector<std::string> getEnemiesOrderedByDifficulty() const { return obterInimigosOrdenadosPorDificuldade(); }

private:
    Bestiary();
    std::map<std::string, SistemaBestiarioEnemyInfo> inimigosBase;
    
    std::set<std::string> vistos;
    std::set<std::string> derrotados;
    std::map<std::string, int> quantidadeDerrotas;
    std::map<std::string, std::set<std::string>> habilidadesVistas;
    std::map<std::string, std::set<std::string>> dropsColetados;
    
    mutable std::mutex mtx;
};
