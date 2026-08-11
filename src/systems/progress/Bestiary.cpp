#include "Bestiary.h"
#include <algorithm>
#include "../../entities/character/Character.h"
#include "../../entities/enemies/goblin/Goblin.h"
#include "../../entities/enemies/slime/Slime.h"
#include "../../entities/enemies/fairy/Fairy.h"
#include "../../entities/enemies/exiled-orc/ExiledOrc.h"
#include "../../entities/enemies/forest-abomination/ForestAbomination.h"
#include "../../entities/enemies/troll/Troll.h"
#include "../../entities/enemies/mimic/Mimic.h"
#include "../../entities/enemies/mahoraga/Mahoraga.h"
#include "../../entities/enemies/ClassBaseEnemy.h"

Bestiary& Bestiary::instancia() {
    static Bestiary inst;
    return inst;
}

Bestiary::Bestiary() {
    initializeInimigos();
}

namespace {
    template<typename T>
    void registrarNoBestiario(std::map<std::string, SistemaBestiarioEnemyInfo>& inimigosBase) {
        T race;
        ClassBaseInimigo classePadrao;
        Attributes attr = race.getRaceAttributes();
        BestiaryInfo info = race.obterBestiaryInfo();
        
        std::vector<std::string> attrTexto = {
            " > Health           : " + std::to_string(attr.health),
            " > Forca          : " + std::to_string(attr.strength),
            " > Destreza       : " + std::to_string(attr.dexterity),
            " > Resistencia    : " + std::to_string(attr.resistance),
            " > Constituicao   : " + std::to_string(attr.constitution),
            " > Inteligencia   : " + std::to_string(attr.intelligence),
            " > Sabedoria      : " + std::to_string(attr.wisdom)
        };

        inimigosBase[race.getRaceName()] = {
            race.getRaceName(), info.map, info.habitat,
            race.getRaceAppearance(),
            info.lore,
            info.funFact,
            attrTexto,
            {classePadrao.getNameHabilidadeClasse() + " | " + classePadrao.getClassAbilityDescription()},
            race.getRaceAbilityName() + " | " + race.getRaceAbilityDescription(),
            info.drops,
            info.difficulty
        };
    }
}

void Bestiary::initializeInimigos() {
    registrarNoBestiario<Goblin>(inimigosBase);
    registrarNoBestiario<Slime>(inimigosBase);
    registrarNoBestiario<Fairy>(inimigosBase);
    registrarNoBestiario<OrkExilado>(inimigosBase);
    registrarNoBestiario<AbominacaoFloresta>(inimigosBase);
    registrarNoBestiario<Troll>(inimigosBase);
    registrarNoBestiario<Mimic>(inimigosBase);
    registrarNoBestiario<Mahoraga>(inimigosBase);
}

void Bestiary::registrarPrimeiraVista(const std::string& nomeInimigo) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) vistos.insert(nomeInimigo);
}

void Bestiary::registrarDerrota(const std::string& nomeInimigo) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) {
        vistos.insert(nomeInimigo);
        derrotados.insert(nomeInimigo);
        quantidadeDerrotas[nomeInimigo]++;
    }
}

void Bestiary::registrarHabilidadeVista(const std::string& nomeInimigo, const std::string& habilidade) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) habilidadesVistas[nomeInimigo].insert(habilidade);
}

void Bestiary::registrarDrop(const std::string& nomeInimigo, const std::string& drop) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) dropsColetados[nomeInimigo].insert(drop);
}

bool Bestiary::estaDescoberto(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    return vistos.count(nomeInimigo) > 0;
}

bool Bestiary::jaDerrotado(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    return derrotados.count(nomeInimigo) > 0;
}

int Bestiary::obterQuantidadeDerrotas(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = quantidadeDerrotas.find(nomeInimigo);
    if (it != quantidadeDerrotas.end()) {
        return it->second;
    }
    return 0;
}

bool Bestiary::jaViuHabilidade(const std::string& nomeInimigo, const std::string& habilidade) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = habilidadesVistas.find(nomeInimigo);
    if (it != habilidadesVistas.end()) return it->second.count(habilidade) > 0;
    return false;
}

bool Bestiary::jaColetouDrop(const std::string& nomeInimigo, const std::string& drop) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = dropsColetados.find(nomeInimigo);
    if (it != dropsColetados.end()) return it->second.count(drop) > 0;
    return false;
}

const SistemaBestiarioEnemyInfo* Bestiary::obterInfo(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = inimigosBase.find(nomeInimigo);
    if (it != inimigosBase.end()) return &it->second;
    return nullptr;
}

std::vector<std::string> Bestiary::obterInimigosOrdenadosPorDificuldade() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> nomes;
    nomes.reserve(inimigosBase.size());
    for (const auto& par : inimigosBase) nomes.push_back(par.first);
    
    std::sort(nomes.begin(), nomes.end(), [this](const std::string& a, const std::string& b) {
        return inimigosBase.at(a).difficulty < inimigosBase.at(b).difficulty;
    });
    
    return nomes;
}

void Bestiary::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto escreverConjunto = [&](const auto& conjunto) {
        out << conjunto.size() << "\n";
        for (const auto& item : conjunto) out << item << "\n";
    };

    escreverConjunto(vistos);
    escreverConjunto(derrotados);

    out << quantidadeDerrotas.size() << "\n";
    for (const auto& [nome, qtd] : quantidadeDerrotas) out << nome << "\n" << qtd << "\n";

    auto escreverMapaConjuntos = [&](const auto& map) {
        out << map.size() << "\n";
        for (const auto& [nome, conjunto] : map) {
            out << nome << "\n";
            escreverConjunto(conjunto);
        }
    };

    escreverMapaConjuntos(habilidadesVistas);
    escreverMapaConjuntos(dropsColetados);
}

void Bestiary::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    vistos.clear();
    derrotados.clear();
    quantidadeDerrotas.clear();
    habilidadesVistas.clear();
    dropsColetados.clear();

    auto lerConjunto = [&](auto& conjunto) {
        size_t size;
        if (!(in >> size)) return false;
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, linha);
            conjunto.insert(linha);
        }
        return true;
    };
    
    if (!lerConjunto(vistos)) return; // Failsafe para saves antigos
    lerConjunto(derrotados);
    
    size_t qtdDerrotasSize;
    if (in >> qtdDerrotasSize) {
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < qtdDerrotasSize; ++i) {
            std::string nome; std::getline(in, nome);
            int qtd; in >> qtd; std::getline(in, linha);
            quantidadeDerrotas[nome] = qtd;
        }
    }

    auto lerMapa = [&](auto& map) {
        size_t size;
        if (!(in >> size)) return;
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < size; ++i) {
            std::string chave; std::getline(in, chave);
            lerConjunto(map[chave]);
        }
    };

    lerMapa(habilidadesVistas);
    lerMapa(dropsColetados);
}






