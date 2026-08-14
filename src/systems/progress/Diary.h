#pragma once

#include <string>
#include <set>
#include <mutex>
#include <fstream>
#include <vector>

class Diary {
public:
    static Diary& instance();
    static Diary& instancia() { return instance(); }

    void registerItem(const std::string& name);
    void registerNPC(const std::string& name);
    void registerRace(const std::string& name);
    void registerClass(const std::string& name);
    void registerAcceptedQuest(const std::string& id);
    void registerCompletedQuest(const std::string& id);
    void registerQuestAccepted(const std::string& id) { registerAcceptedQuest(id); }
    void registerQuestCompleted(const std::string& id) { registerCompletedQuest(id); }

    bool isItemDiscovered(const std::string& name) const;
    bool isNPCDiscovered(const std::string& name) const;
    bool isRaceDiscovered(const std::string& name) const;
    bool isClassDiscovered(const std::string& name) const;
    bool isQuestAccepted(const std::string& id) const;
    bool isQuestCompleted(const std::string& id) const;

    std::vector<std::string> getDiscoveredItems() const;
    std::vector<std::string> getDiscoveredNPCs() const;
    std::vector<std::string> getDiscoveredRaces() const;
    std::vector<std::string> getDiscoveredClasses() const;
    std::vector<std::string> getAcceptedQuests() const;
    std::vector<std::string> getCompletedQuests() const;

    void save(std::ofstream& out) const;
    void load(std::ifstream& in);

    // Aliases legados em portugues
    void registrarItem(const std::string& nomeItem) { registerItem(nomeItem); }
    void registrarNPC(const std::string& nomeNPC) { registerNPC(nomeNPC); }
    void registrarRaca(const std::string& nomeRaca) { registerRace(nomeRaca); }
    void registrarClasse(const std::string& nomeClasse) { registerClass(nomeClasse); }
    void registrarMissaoAceita(const std::string& idMissao) { registerAcceptedQuest(idMissao); }
    void registrarMissaoConcluida(const std::string& idMissao) { registerCompletedQuest(idMissao); }

    bool itemDescoberto(const std::string& nomeItem) const { return isItemDiscovered(nomeItem); }
    bool npcDescoberto(const std::string& nomeNPC) const { return isNPCDiscovered(nomeNPC); }
    bool racaDescoberta(const std::string& nomeRaca) const { return isRaceDiscovered(nomeRaca); }
    bool classeDescoberta(const std::string& nomeClasse) const { return isClassDiscovered(nomeClasse); }
    bool missaoAceita(const std::string& idMissao) const { return isQuestAccepted(idMissao); }
    bool missaoConcluida(const std::string& idMissao) const { return isQuestCompleted(idMissao); }

    std::vector<std::string> obterItensDescobertos() const { return getDiscoveredItems(); }
    std::vector<std::string> obterNPCsDescobertos() const { return getDiscoveredNPCs(); }
    std::vector<std::string> obterRacasDescobertas() const { return getDiscoveredRaces(); }
    std::vector<std::string> obterClassesDescobertas() const { return getDiscoveredClasses(); }
    std::vector<std::string> obterMissoesAceitas() const { return getAcceptedQuests(); }
    std::vector<std::string> obterMissoesConcluidas() const { return getCompletedQuests(); }

    void salvar(std::ofstream& out) const { save(out); }
    void carregar(std::ifstream& in) { load(in); }

private:
    Diary();
    
    std::set<std::string> discoveredItems;
    std::set<std::string> discoveredNPCs;
    std::set<std::string> discoveredRaces;
    std::set<std::string> discoveredClasses;
    std::set<std::string> acceptedQuests;
    std::set<std::string> completedQuests;

    mutable std::mutex mtx;
};
