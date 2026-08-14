#include "Diary.h"

Diary& Diary::instance() {
    static Diary inst;
    return inst;
}

Diary::Diary() {}

void Diary::registerItem(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    discoveredItems.insert(name);
}

void Diary::registerNPC(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    discoveredNPCs.insert(name);
}

void Diary::registerRace(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    discoveredRaces.insert(name);
}

void Diary::registerClass(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    discoveredClasses.insert(name);
}

void Diary::registerAcceptedQuest(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx);
    acceptedQuests.insert(id);
}

void Diary::registerCompletedQuest(const std::string& id) {
    std::lock_guard<std::mutex> lock(mtx);
    completedQuests.insert(id);
}

bool Diary::isItemDiscovered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mtx);
    return discoveredItems.count(name) > 0;
}

bool Diary::isNPCDiscovered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mtx);
    return discoveredNPCs.count(name) > 0;
}

bool Diary::isRaceDiscovered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mtx);
    return discoveredRaces.count(name) > 0;
}

bool Diary::isClassDiscovered(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mtx);
    return discoveredClasses.count(name) > 0;
}

bool Diary::isQuestAccepted(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mtx);
    return acceptedQuests.count(id) > 0;
}

bool Diary::isQuestCompleted(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mtx);
    return completedQuests.count(id) > 0;
}

std::vector<std::string> Diary::getDiscoveredItems() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(discoveredItems.begin(), discoveredItems.end());
}

std::vector<std::string> Diary::getDiscoveredNPCs() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(discoveredNPCs.begin(), discoveredNPCs.end());
}

std::vector<std::string> Diary::getDiscoveredRaces() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(discoveredRaces.begin(), discoveredRaces.end());
}

std::vector<std::string> Diary::getDiscoveredClasses() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(discoveredClasses.begin(), discoveredClasses.end());
}

std::vector<std::string> Diary::getAcceptedQuests() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(acceptedQuests.begin(), acceptedQuests.end());
}

std::vector<std::string> Diary::getCompletedQuests() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(completedQuests.begin(), completedQuests.end());
}

void Diary::save(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto writeSet = [&](const std::set<std::string>& set) {
        out << set.size() << "\n";
        for (const auto& item : set) out << item << "\n";
    };

    writeSet(discoveredItems);
    writeSet(discoveredNPCs);
    writeSet(discoveredRaces);
    writeSet(discoveredClasses);
    writeSet(acceptedQuests);
    writeSet(completedQuests);
}

void Diary::load(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    
    discoveredItems.clear();
    discoveredNPCs.clear();
    discoveredRaces.clear();
    discoveredClasses.clear();
    acceptedQuests.clear();
    completedQuests.clear();

    auto readSet = [&](std::set<std::string>& set) {
        size_t size;
        if (!(in >> size)) return false;
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, line);
            set.insert(line);
        }
        return true;
    };
    
    readSet(discoveredItems);
    readSet(discoveredNPCs);
    readSet(discoveredRaces);
    readSet(discoveredClasses);
    readSet(acceptedQuests);
    readSet(completedQuests);
}
