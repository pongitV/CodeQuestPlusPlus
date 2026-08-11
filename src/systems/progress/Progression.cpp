#include "Progression.h"
#include "../../entities/character/Character.h"
#include "ProgressionFlags.h"
#include "Diary.h"

Progression& Progression::instancia() {
    static Progression inst;
    return inst;
}

Progression::Progression() {}

void Progression::definirFlag(const std::string& chave, bool valor) {
    std::lock_guard<std::mutex> lock(mtx);
    flags[chave] = valor;
}

bool Progression::obterFlag(const std::string& chave) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = flags.find(chave);
    if (it != flags.end()) return it->second;
    return false;
}

int Progression::obterProgressoVila(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool vilaNpcsEncontrados = currentPlayer->getLevel() > 1 || obterFlag(Flags::Vila_NPCs); 
    bool vilaInimigosDerrotados = currentPlayer->getXpAtual() > 0 || currentPlayer->getLevel() > 1 || obterFlag(Flags::Vila_Inimigos);
    bool vilaMissaoConcluida = obterFlag(Flags::Vila_ConviteReal) || currentPlayer->obterInventario()->contarItem("Convite Real") > 0;
    return (vilaNpcsEncontrados ? 33 : 0) + (vilaInimigosDerrotados ? 33 : 0) + (vilaMissaoConcluida ? 34 : 0);
}

int Progression::obterProgressoFloresta(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool florestaNpcsEncontrados = currentPlayer->obterLabirintoDesbloqueado() || obterFlag(Flags::Floresta_NPCs);
    bool florestaInimigosDerrotados = obterFlag(Flags::Floresta_MahoragaDerrotado);
    bool florestaMissaoConcluida = obterFlag(Flags::Floresta_MissaoMorgana);
    return (florestaNpcsEncontrados ? 33 : 0) + (florestaInimigosDerrotados ? 33 : 0) + (florestaMissaoConcluida ? 34 : 0);
}

int Progression::obterProgressoPonteReino(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool trollDerrotado = obterFlag(Flags::PonteReino_TrollDerrotado);
    return (trollDerrotado ? 34 : 0) + (obterFlag(Flags::PonteReino_NPCs) ? 33 : 0) + (obterFlag(Flags::PonteReino_Inimigos) ? 33 : 0);
}

int Progression::obterProgressoReino(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool visitou = obterFlag(Flags::Visitou_Reino);
    bool conversouPriest = Diary::instancia().npcDescoberto("Priest Benedito");
    return (visitou ? 50 : 0) + (conversouPriest ? 50 : 0);
}

void Progression::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    out << flags.size() << "\n";
    for (const auto& [chave, valor] : flags) out << chave << "\n" << (valor ? 1 : 0) << "\n";
}

void Progression::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    flags.clear();
    size_t size;
    if (in >> size) {
        std::string lixo; std::getline(in, lixo); // consome a quebra de linha
        for (size_t i = 0; i < size; ++i) { std::string chave; std::getline(in, chave); int valor; in >> valor; std::getline(in, lixo); flags[chave] = (valor == 1); }
    }

    // --- RETROCOMPATIBILIDADE DE SAVES ANTIGOS ---
    // Evita que saves antigos (anteriores a atualizacao) percam o acesso a Viagem Rapida
    auto itFloresta = flags.find("Visitou_Floresta");
    auto itPonteReino = flags.find("Visitou_PonteReino");
    if ((itFloresta != flags.end() && itFloresta->second) || 
        (itPonteReino != flags.end() && itPonteReino->second)) {
        flags["Mapas_Descobertos"] = true;
    }
}
