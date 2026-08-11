#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <mutex>

class Character;

// GerenciadorDeProgresso gerencia as flags de progresso, marcos de missoes e porcentagens de exploracao de zonas.
class Progression {
private:
    std::unordered_map<std::string, bool> flags;
    mutable std::mutex mtx;

    Progression();
public:
    static Progression& instancia();
    static Progression& obterInstancia() { return instancia(); }
    static Progression& getInstance() { return instancia(); }
    static Progression& instance() { return instancia(); }

    void definirFlag(const std::string& chave, bool valor);
    bool obterFlag(const std::string& chave) const;

    void setFlag(const std::string& key, bool val) { definirFlag(key, val); }
    bool getFlag(const std::string& key) const { return obterFlag(key); }

    // Calculos dinamicos de progresso combinando o estado do jogador e as flags salvas
    int obterProgressoVila(Character* currentPlayer) const;
    int obterProgressoFloresta(Character* currentPlayer) const;
    int obterProgressoPonteReino(Character* currentPlayer) const;
    int obterProgressoReino(Character* currentPlayer) const;

    int getVillageProgress(Character* player) const { return obterProgressoVila(player); }
    int getForestProgress(Character* player) const { return obterProgressoFloresta(player); }
    int getKingdomBridgeProgress(Character* player) const { return obterProgressoPonteReino(player); }
    int getKingdomProgress(Character* player) const { return obterProgressoReino(player); }

    // Sincronizacao do sistema de salvamento
    void salvar(std::ofstream& out) const;
    void carregar(std::ifstream& in);
    void save(std::ofstream& out) const { salvar(out); }
    void load(std::ifstream& in) { carregar(in); }
};

using GerenciadorDeProgresso = Progression;
using ProgressionManager = Progression;
using Progression = Progression;
