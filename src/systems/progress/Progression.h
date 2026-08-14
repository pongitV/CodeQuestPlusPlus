#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <mutex>

class Character;

// Gerenciador de progresso, marcos de missoes e porcentagens de exploracao de zonas.
class Progression {
private:
    std::unordered_map<std::string, bool> flags;
    mutable std::mutex mtx;

    Progression();
public:
    static Progression& instance();
    static Progression& getInstance() { return instance(); }
    static Progression& instancia() { return instance(); }
    static Progression& obterInstancia() { return instance(); }

    void setFlag(const std::string& key, bool val);
    bool getFlag(const std::string& key) const;

    void definirFlag(const std::string& chave, bool valor) { setFlag(chave, valor); }
    bool obterFlag(const std::string& chave) const { return getFlag(chave); }

    // Calculos dinamicos de progresso combinando o estado do jogador e as flags salvas
    int getVillageProgress(Character* player) const;
    int getForestProgress(Character* player) const;
    int getKingdomBridgeProgress(Character* player) const;
    int getKingdomProgress(Character* player) const;

    int obterProgressoVila(Character* currentPlayer) const { return getVillageProgress(currentPlayer); }
    int obterProgressoFloresta(Character* currentPlayer) const { return getForestProgress(currentPlayer); }
    int obterProgressoPonteReino(Character* currentPlayer) const { return getKingdomBridgeProgress(currentPlayer); }
    int obterProgressoReino(Character* currentPlayer) const { return getKingdomProgress(currentPlayer); }

    // Sincronizacao do sistema de salvamento
    void save(std::ofstream& out) const;
    void load(std::ifstream& in);
    void salvar(std::ofstream& out) const { save(out); }
    void carregar(std::ifstream& in) { load(in); }
};

using GerenciadorDeProgresso = Progression;
using ProgressionManager = Progression;
