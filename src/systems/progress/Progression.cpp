#include "Progression.h"
#include "../../entities/character/Character.h"
#include "ProgressionFlags.h"
#include "Diary.h"

Progression& Progression::instance() {
    static Progression inst;
    return inst;
}

Progression::Progression() {}

void Progression::setFlag(const std::string& key, bool val) {
    std::lock_guard<std::mutex> lock(mtx);
    flags[key] = val;
}

bool Progression::getFlag(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = flags.find(key);
    if (it != flags.end()) return it->second;
    return false;
}

int Progression::getVillageProgress(Character* player) const {
    if (!player) return 0;
    bool villageNpcsFound = player->getLevel() > 1 || getFlag(Flags::Village_NPCs); 
    bool villageEnemiesDefeated = player->getCurrentXp() > 0 || player->getLevel() > 1 || getFlag(Flags::Village_Enemies);
    bool villageQuestCompleted = getFlag(Flags::Village_RoyalInvitation) || player->getInventory()->countItem("Convite Real") > 0;
    return (villageNpcsFound ? 33 : 0) + (villageEnemiesDefeated ? 33 : 0) + (villageQuestCompleted ? 34 : 0);
}

int Progression::getForestProgress(Character* player) const {
    if (!player) return 0;
    bool forestNpcsFound = player->isLabyrinthUnlocked() || getFlag(Flags::Forest_NPCs);
    bool forestEnemiesDefeated = getFlag(Flags::Forest_MahoragaDefeated);
    bool forestQuestCompleted = getFlag(Flags::Forest_MorganaQuest);
    return (forestNpcsFound ? 33 : 0) + (forestEnemiesDefeated ? 33 : 0) + (forestQuestCompleted ? 34 : 0);
}

int Progression::getKingdomBridgeProgress(Character* player) const {
    if (!player) return 0;
    bool trollDefeated = getFlag(Flags::KingdomBridge_TrollDefeated);
    return (trollDefeated ? 34 : 0) + (getFlag(Flags::KingdomBridge_NPCs) ? 33 : 0) + (getFlag(Flags::KingdomBridge_Enemies) ? 33 : 0);
}

int Progression::getKingdomProgress(Character* player) const {
    if (!player) return 0;
    bool visited = getFlag(Flags::Visited_Kingdom);
    bool talkedPriest = Diary::instance().isNPCDiscovered("Priest Benedito");
    return (visited ? 50 : 0) + (talkedPriest ? 50 : 0);
}

void Progression::save(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    out << flags.size() << "\n";
    for (const auto& [key, val] : flags) out << key << "\n" << (val ? 1 : 0) << "\n";
}

void Progression::load(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    flags.clear();
    size_t size;
    if (in >> size) {
        std::string trash; std::getline(in, trash); // consome a quebra de linha
        for (size_t i = 0; i < size; ++i) { std::string key; std::getline(in, key); int val; in >> val; std::getline(in, trash); flags[key] = (val == 1); }
    }

    // --- RETROCOMPATIBILIDADE DE SAVES ANTIGOS ---
    // Evita que saves antigos (anteriores a atualizacao) percam o acesso a Viagem Rapida
    auto itForest = flags.find("Visitou_Floresta");
    auto itKingdomBridge = flags.find("Visitou_PonteReino");
    if ((itForest != flags.end() && itForest->second) || 
        (itKingdomBridge != flags.end() && itKingdomBridge->second)) {
        flags["Mapas_Descobertos"] = true;
    }
}
