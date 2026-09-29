#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "../../entities/character/Character.h"
#include "../interfaces/IMap.h"

// Representa a zona inicial da vila no mapa do mundo.
// Controls zone geometry, tile collision matrix, sub-map transitions, and NPC interactions.
class Mapa1Vila final : public IMap 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Character* currentPlayer;
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;

    std::unordered_map<char, std::unique_ptr<VillageInteraction>> interacoes;

    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;

    std::vector<std::string> matrizDoMapaDaCavernaSalva;
    std::vector<std::string> matrizDoMapaDoSpawnSalva;
    std::vector<std::string> mapaBaseDaVila;

    // Quest and exploration state flags
    bool bjornResgatado;
    bool cavernaJaFoiVisitada;
    bool spawnJaFoiVisitado;

    NextMapTransition proximoMapa;
    bool veioDaFloresta;

public:
    explicit Mapa1Vila(Character* personagemJogador);
    ~Mapa1Vila() override;

    std::string getTitle() const override { return tituloDoMapaAtual; }
    std::string obterTitulo() const override { return getTitle(); }

    NextMapTransition iniciarLoopDeExploracao() override;
    NextMapTransition startExplorationLoop() override { return iniciarLoopDeExploracao(); }

private:
    void initializeInteracoes();
};

using VillageMap = Mapa1Vila;
