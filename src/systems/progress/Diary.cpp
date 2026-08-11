#include "Diary.h"

Diary& Diary::instancia() {
    static Diary inst;
    return inst;
}

Diary::Diary() {}

void Diary::registrarItem(const std::string& nomeItem) {
    std::lock_guard<std::mutex> lock(mtx);
    itensDescobertos.insert(nomeItem);
}

void Diary::registrarNPC(const std::string& nomeNPC) {
    std::lock_guard<std::mutex> lock(mtx);
    npcsDescobertos.insert(nomeNPC);
}

void Diary::registrarRaca(const std::string& nomeRaca) {
    std::lock_guard<std::mutex> lock(mtx);
    racasDescobertas.insert(nomeRaca);
}

void Diary::registrarClasse(const std::string& nomeClasse) {
    std::lock_guard<std::mutex> lock(mtx);
    classesDescobertas.insert(nomeClasse);
}

void Diary::registrarMissaoAceita(const std::string& idMissao) {
    std::lock_guard<std::mutex> lock(mtx);
    missoesAceitas.insert(idMissao);
}

void Diary::registrarMissaoConcluida(const std::string& idMissao) {
    std::lock_guard<std::mutex> lock(mtx);
    missoesConcluidas.insert(idMissao);
}

bool Diary::itemDescoberto(const std::string& nomeItem) const {
    std::lock_guard<std::mutex> lock(mtx);
    return itensDescobertos.count(nomeItem) > 0;
}

bool Diary::npcDescoberto(const std::string& nomeNPC) const {
    std::lock_guard<std::mutex> lock(mtx);
    return npcsDescobertos.count(nomeNPC) > 0;
}

bool Diary::racaDescoberta(const std::string& nomeRaca) const {
    std::lock_guard<std::mutex> lock(mtx);
    return racasDescobertas.count(nomeRaca) > 0;
}

bool Diary::classeDescoberta(const std::string& nomeClasse) const {
    std::lock_guard<std::mutex> lock(mtx);
    return classesDescobertas.count(nomeClasse) > 0;
}

bool Diary::missaoAceita(const std::string& idMissao) const {
    std::lock_guard<std::mutex> lock(mtx);
    return missoesAceitas.count(idMissao) > 0;
}

bool Diary::missaoConcluida(const std::string& idMissao) const {
    std::lock_guard<std::mutex> lock(mtx);
    return missoesConcluidas.count(idMissao) > 0;
}

std::vector<std::string> Diary::obterItensDescobertos() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(itensDescobertos.begin(), itensDescobertos.end());
}

std::vector<std::string> Diary::obterNPCsDescobertos() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(npcsDescobertos.begin(), npcsDescobertos.end());
}

std::vector<std::string> Diary::obterRacasDescobertas() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(racasDescobertas.begin(), racasDescobertas.end());
}

std::vector<std::string> Diary::obterClassesDescobertas() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(classesDescobertas.begin(), classesDescobertas.end());
}

std::vector<std::string> Diary::obterMissoesAceitas() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(missoesAceitas.begin(), missoesAceitas.end());
}

std::vector<std::string> Diary::obterMissoesConcluidas() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(missoesConcluidas.begin(), missoesConcluidas.end());
}

void Diary::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto escreverConjunto = [&](const std::set<std::string>& conjunto) {
        out << conjunto.size() << "\n";
        for (const auto& item : conjunto) out << item << "\n";
    };

    escreverConjunto(itensDescobertos);
    escreverConjunto(npcsDescobertos);
    escreverConjunto(racasDescobertas);
    escreverConjunto(classesDescobertas);
    escreverConjunto(missoesAceitas);
    escreverConjunto(missoesConcluidas);
}

void Diary::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    
    itensDescobertos.clear();
    npcsDescobertos.clear();
    racasDescobertas.clear();
    classesDescobertas.clear();
    missoesAceitas.clear();
    missoesConcluidas.clear();

    auto lerConjunto = [&](std::set<std::string>& conjunto) {
        size_t size;
        if (!(in >> size)) return false;
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, linha);
            conjunto.insert(linha);
        }
        return true;
    };
    
    lerConjunto(itensDescobertos);
    lerConjunto(npcsDescobertos);
    lerConjunto(racasDescobertas);
    lerConjunto(classesDescobertas);
    lerConjunto(missoesAceitas);
    lerConjunto(missoesConcluidas);
}
